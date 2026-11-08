#ifndef EDepSim_TrackingMessages_hh
#define EDepSim_TrackingMessages_hh 1

// EDepSimTrackingMessages.hh
//
// The framework-agnostic request/reply protocol carried over an
// EDepSim::TrackingChannel between an EDepSim::TrackingService (called from any
// thread) and an EDepSim::TrackingWorker (the one dedicated Geant4 thread).
//
// No Geant4 types appear here -- only the edep-sim i/o data model (TG4Event,
// TG4PrimaryVertexContainer) and the standard library -- so this header, the
// channel, and the service compile without Geant4.

#include "TG4Event.h"
#include "TG4PrimaryVertex.h"   // TG4PrimaryVertexContainer

#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

// Forward declaration only: the generator factory below is typed on a pointer
// to this Geant4 base class, so this header (and the whole front end) stays
// Geant4-header-free.  Callers that actually build a generator include Geant4.
class G4VPrimaryGenerator;

namespace EDepSim {

    // ------------------------------------------------------------- requests

    /// A caller-supplied factory that constructs a G4VPrimaryGenerator.  It is
    /// invoked BY THE WORKER, ON THE DEDICATED GEANT4 THREAD, so that any
    /// thread-local Geant4 state the generator's constructor touches
    /// (G4ParticleTable, G4UImanager/messengers, G4Allocator pools) is bound to
    /// the one thread that runs it.  Constructing the generator on another
    /// thread and handing it over would break that affinity, so the interface
    /// takes a factory rather than a ready-made generator.  Returns a
    /// heap-allocated generator; the worker takes ownership.  (EDepSim's own
    /// EDepSim::PrimaryGenerator IS a G4VPrimaryGenerator, so a factory may
    /// return one of those to use the full VKinematicsGenerator machinery.)
    using GeneratorFactory = std::function<G4VPrimaryGenerator*()>;

    /// One-time setup: build the run manager for the named physics list, read
    /// the GDML geometry, apply macro commands, and /edep/update.
    struct InitializeRequest {
        std::string physicsList;              ///< passed to EDepSim::CreateRunManager
        std::string gdml;                     ///< GDML file to /edep/gdml/read (may be empty)
        std::vector<std::string> macros;      ///< raw G4 UI command lines, applied in order
        bool validateGeometry{false};         ///< run /edep/validateGeometry before /edep/update

        /// Optional.  If null, the worker installs its built-in
        /// PrimaryVertexGenerator and simulate(TG4PrimaryVertexContainer, id)
        /// drives it.  If set, the worker constructs THIS generator on the
        /// Geant4 thread instead; then drive it with simulate(id) (no
        /// container).
        GeneratorFactory generatorFactory;
    };

    /// Apply additional raw G4 UI command lines after initialization.
    struct ApplyMacroRequest {
        std::vector<std::string> commands;
    };

    /// Track exactly one event.  event_id also seeds the RNG so a given id
    /// reproduces a given event.  `primaries` is the native edep-sim i/o type
    /// (values in Geant4/CLHEP units); it may be null, in which case no
    /// container is fed and the installed generator supplies the primaries on
    /// its own (the custom-generatorFactory mode).
    struct SimulateRequest {
        std::shared_ptr<const TG4PrimaryVertexContainer> primaries;
        unsigned long eventId{0};
    };

    /// Ask the worker to stop looping (teardown happens on the worker thread).
    struct ShutdownRequest {};

    using Request = std::variant<InitializeRequest,
                                 ApplyMacroRequest,
                                 SimulateRequest,
                                 ShutdownRequest>;

    // -------------------------------------------------------------- replies

    struct Empty {};
    struct SimulateReply { std::shared_ptr<TG4Event> event; };

    using ReplyValue = std::variant<Empty, SimulateReply>;

    /// A reply carries either a value or, if the worker caught an exception
    /// while servicing the request, that exception (rethrown to the caller).
    struct Reply {
        ReplyValue value;
        std::exception_ptr error;
    };

    using Completion = std::function<void(Reply)>;

    /// A unit of work delivered through the channel.  `done` may be null for
    /// fire-and-forget messages (e.g. ShutdownRequest from a destructor).
    struct Message {
        Request request;
        Completion done;
    };

} // namespace EDepSim
#endif
