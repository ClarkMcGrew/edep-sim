// EDepSimTrackingServiceGeneratorTest.cc
//
// Custom-generator path: the caller supplies a GeneratorFactory instead of a
// TG4PrimaryVertexContainer.  This test also demonstrates the thread-affinity
// guarantee -- the generator is CONSTRUCTED on the dedicated Geant4 thread, not
// on the caller's thread -- by recording the thread id inside the generator
// constructor and comparing it to the main thread.
//
// Usage: EDepSimTrackingServiceGeneratorTest <geometry.gdml> [physics-list]

#include "EDepSimTrackingService.hh"

#include "TG4Event.h"

#include <G4Event.hh>
#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>
#include <G4VPrimaryGenerator.hh>

#include <atomic>
#include <cmath>
#include <cstdio>
#include <string>
#include <thread>

namespace {

    // Where the generator's constructor ran (set on the worker thread).
    std::atomic<std::thread::id> g_ctor_thread;

    // A trivial caller-owned generator: one forward muon at the origin.
    class MuonGun : public G4VPrimaryGenerator {
    public:
        MuonGun() {
            // Runs on whatever thread constructs us -- must be the G4 thread.
            g_ctor_thread.store(std::this_thread::get_id());
        }
        void GeneratePrimaryVertex(G4Event* event) override {
            auto* vtx = new G4PrimaryVertex(0.0, 0.0, 0.0, 0.0);
            const double p = 1000.0, m = 105.6583745;
            vtx->SetPrimary(new G4PrimaryParticle(
                13, 0.0, 0.0, p, std::sqrt(p * p + m * m)));
            event->AddPrimaryVertex(vtx);
        }
    };

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: %s <geometry.gdml> [physics-list]\n", argv[0]);
        return 2;
    }
    const std::string gdml = argv[1];
    const std::string physicsList = (argc > 2) ? argv[2] : std::string{};

    int failures = 0;
    auto check = [&](bool ok, const char* what) {
        std::printf("%s %s\n", ok ? "ok:  " : "FAIL:", what);
        if (!ok) ++failures;
    };

    const std::thread::id main_thread = std::this_thread::get_id();

    {
        EDepSim::TrackingService svc;

        // The factory is invoked by the worker ON THE GEANT4 THREAD.
        svc.initialize(physicsList, gdml,
                       EDepSim::GeneratorFactory([] { return new MuonGun(); }));

        auto ev = svc.simulate(/*eventId=*/55);   // no container: the gun drives

        check(static_cast<bool>(ev), "custom-generator simulate returned a TG4Event");
        if (ev) {
            std::printf("     primaries=%zu trajectories=%zu eventId=%d\n",
                        ev->Primaries.size(), ev->Trajectories.size(),
                        ev->EventId);
            check(ev->Trajectories.size() >= 1, "custom-generator event was tracked");
            check(ev->EventId == 55, "custom-generator EventId stamped");
        }

        // The whole point: the generator was built on the worker thread.
        check(g_ctor_thread.load() != std::thread::id{},
              "generator constructor ran");
        check(g_ctor_thread.load() != main_thread,
              "generator was constructed on the Geant4 thread, not the caller");
    }

    if (failures) {
        std::printf("\n%d FAILURE(S)\n", failures);
        return 1;
    }
    std::printf("\nTrackingService custom-generator test passed.\n");
    return 0;
}
