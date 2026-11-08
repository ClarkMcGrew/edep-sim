// EDepSimTrackingWorker.cc

#include "EDepSimTrackingWorker.hh"

#include "EDepSimTrackingChannel.hh"
#include "EDepSimPrimaryVertexGenerator.hh"

#include "EDepSimCreateRunManager.hh"
#include "EDepSimPersistencyManager.hh"
#include "EDepSimUserPrimaryGeneratorAction.hh"

#include <G4GeometryManager.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4RunManager.hh>
#include <G4SolidStore.hh>
#include <G4UImanager.hh>
#include <Randomize.hh>

#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

    // Apply one raw G4 UI command, skipping blank lines and '#' comments so a
    // multi-line macro can be delivered as a vector of lines.
    void apply_command(G4UImanager& ui, const std::string& line) {
        const auto first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return;   // blank
        if (line[first] == '#') return;           // comment
        ui.ApplyCommand(line);
    }

} // namespace

namespace EDepSim {

    // The Geant4 objects: created, used and destroyed only on the worker thread.
    struct TrackingWorker::Impl {
        std::unique_ptr<G4RunManager> runManager;
        std::unique_ptr<EDepSim::PersistencyManager> persistency;
        // The installed generator, owned by us.  In the default mode it is our
        // PrimaryVertexGenerator and `containerGenerator` aliases it (so
        // simulate() can Feed a container).  In the custom-factory mode it is
        // whatever the caller's factory built and `containerGenerator` is null.
        std::unique_ptr<G4VPrimaryGenerator> generator;
        EDepSim::PrimaryVertexGenerator* containerGenerator{nullptr};
        EDepSim::UserPrimaryGeneratorAction* action{nullptr};  // owned by runManager
        bool shutdownDone{false};
    };

    TrackingWorker::TrackingWorker() : fImpl(std::make_unique<Impl>()) {}

    TrackingWorker::~TrackingWorker() {
        // The Geant4 objects were released in doShutdown(); nothing G4 is deleted
        // here.  doShutdown() is idempotent and already ran on this thread.
        doShutdown();
    }

    void TrackingWorker::run(TrackingChannel& channel) {
        while (auto msg = channel.recv()) {
            Reply rep;
            bool quit = false;
            try {
                rep.value = std::visit(
                    [&](auto&& r) -> ReplyValue {
                        using T = std::decay_t<decltype(r)>;
                        if constexpr (std::is_same_v<T, InitializeRequest>) {
                            doInitialize(r);
                            return Empty{};
                        }
                        else if constexpr (std::is_same_v<T, ApplyMacroRequest>) {
                            doMacros(r);
                            return Empty{};
                        }
                        else if constexpr (std::is_same_v<T, SimulateRequest>) {
                            return SimulateReply{doSimulate(r)};
                        }
                        else {  // ShutdownRequest
                            quit = true;
                            return Empty{};
                        }
                    },
                    msg->request);
            }
            catch (...) {
                rep.error = std::current_exception();
            }
            if (msg->done) { msg->done(std::move(rep)); }
            if (quit) { break; }
        }

        // Teardown runs on this thread whether we stopped on a ShutdownRequest or
        // on the channel closing.
        doShutdown();
    }

    void TrackingWorker::doInitialize(const InitializeRequest& req) {
        if (fInitialized) {
            throw std::runtime_error(
                "EDepSim::TrackingWorker: already initialized");
        }

        // Build the edep-sim run manager for the requested physics list.  The
        // physics list is a construction-time choice, not a runtime macro.
        fImpl->runManager.reset(EDepSim::CreateRunManager(req.physicsList));

        // Install the BASE (not Root) persistency manager: its Store() fills the
        // TG4Event summary (fEventSummary) each event but writes no ROOT file and
        // builds no TGeo.  Creating it registers it as the G4 persistency
        // singleton, so G4RunManager::AnalyzeEvent calls its Store().
        fImpl->persistency = std::make_unique<EDepSim::PersistencyManager>();

        // Construct the generator ON THIS THREAD and inject it.  Either the
        // caller's factory (custom kinematics) or, by default, our
        // PrimaryVertexGenerator driven by simulate(TG4PrimaryVertexContainer).
        // Building here is what preserves Geant4 thread affinity: whatever
        // thread-local state the generator's constructor touches is bound to the
        // one thread that will run it.
        if (req.generatorFactory) {
            G4VPrimaryGenerator* g = req.generatorFactory();
            if (!g) {
                throw std::runtime_error(
                    "EDepSim::TrackingWorker: generatorFactory returned null");
            }
            fImpl->generator.reset(g);
            fImpl->containerGenerator = nullptr;
        }
        else {
            auto* g = new EDepSim::PrimaryVertexGenerator();
            fImpl->generator.reset(g);
            fImpl->containerGenerator = g;
        }

        // The action is owned by the run manager;
        // GetUserPrimaryGeneratorAction() is const, hence the cast.
        fImpl->action = const_cast<EDepSim::UserPrimaryGeneratorAction*>(
            static_cast<const EDepSim::UserPrimaryGeneratorAction*>(
                fImpl->runManager->GetUserPrimaryGeneratorAction()));
        if (!fImpl->action) {
            throw std::runtime_error(
                "EDepSim::TrackingWorker: run manager has no "
                "UserPrimaryGeneratorAction");
        }
        fImpl->action->AddGenerator(fImpl->generator.get());

        G4UImanager* ui = G4UImanager::GetUIpointer();

        // Geometry from GDML (if provided).
        if (!req.gdml.empty()) {
            ui->ApplyCommand("/edep/gdml/read " + req.gdml);
        }

        // edep-sim defaults: ionization model, trajectory-save thresholds, etc.
        ui->ApplyCommand("/edep/control edepsim-defaults 1.0");

        // Caller-supplied macro/tuning commands.
        for (const std::string& cmd : req.macros) {
            apply_command(*ui, cmd);
        }

        if (req.validateGeometry) {
            ui->ApplyCommand("/edep/validateGeometry");
        }

        // Initialize geometry + physics (triggers /run/initialize).
        ui->ApplyCommand("/edep/update");

        fInitialized = true;
    }

    void TrackingWorker::doMacros(const ApplyMacroRequest& req) {
        G4UImanager* ui = G4UImanager::GetUIpointer();
        for (const std::string& cmd : req.commands) {
            apply_command(*ui, cmd);
        }
    }

    std::shared_ptr<TG4Event>
    TrackingWorker::doSimulate(const SimulateRequest& req) {
        if (!fInitialized) {
            throw std::runtime_error(
                "EDepSim::TrackingWorker: simulate before initialize");
        }

        // Reseed from the event id so a given id reproduces a given event
        // (decision ddm-2ha.5).  Match EDepSim::UserRunAction::SetSeed's
        // positivity handling.
        long seed = static_cast<long>(req.eventId);
        if (seed < 0) { seed = -seed; }
        G4Random::setTheSeed(seed);

        // Deliver the primaries.  With a container, feed our built-in generator;
        // a null container means the installed custom generator supplies its own
        // primaries.  Feeding a container in custom-generator mode is a usage
        // error (there is no container generator to receive it).
        if (req.primaries) {
            if (!fImpl->containerGenerator) {
                throw std::runtime_error(
                    "EDepSim::TrackingWorker: simulate(container) but this "
                    "worker was initialized with a custom generatorFactory; "
                    "use simulate(id) instead");
            }
            fImpl->containerGenerator->Feed(req.primaries);
        }

        // Run exactly one Geant4 event.
        G4UImanager::GetUIpointer()->ApplyCommand("/run/beamOn 1");

        // The persistency manager's Store() has filled fEventSummary; copy it out
        // (it is overwritten by the next event) and stamp the caller's event id.
        auto out =
            std::make_shared<TG4Event>(fImpl->persistency->GetEventSummary());
        out->EventId = static_cast<int>(req.eventId);
        return out;
    }

    void TrackingWorker::doShutdown() {
        if (!fImpl || fImpl->shutdownDone) { return; }
        fImpl->shutdownDone = true;

        if (fInitialized) {
            // Empty Geant4's GLOBAL geometry stores HERE, on the thread that
            // BUILT the volumes.  This is the essential step: those stores are
            // static singletons torn down by main-thread destructors at process
            // exit, and if they still hold worker-built volumes the main thread
            // deletes them cross-thread -> crash in ~G4PVPlacement/GetRotation.
            // OpenGeometry() is required first because the geometry is closed
            // (optimized) after /run/initialize.  With the stores emptied here,
            // the run manager can then be destroyed safely on this same thread
            // (verified: deleting it WITHOUT this Clean segfaults at exit, WITH
            // it exits cleanly -- so the historical "leak the run manager" was
            // unnecessary once the stores are cleaned).
            G4GeometryManager::GetInstance()->OpenGeometry();
            G4PhysicalVolumeStore::Clean();
            G4LogicalVolumeStore::Clean();
            G4SolidStore::Clean();

            // Destroy the thread-affine Geant4 objects on this thread, in the
            // order used by app/edepSim.cc: persistency, then run manager.  The
            // run manager owns and deletes the UserPrimaryGeneratorAction; that
            // action only borrows our generator (it does not delete it), so we
            // delete the generator ourselves, last.
            if (fImpl->persistency) { fImpl->persistency->Close(); }
            fImpl->persistency.reset();
            fImpl->runManager.reset();
            fImpl->generator.reset();
        }
    }

} // namespace EDepSim
