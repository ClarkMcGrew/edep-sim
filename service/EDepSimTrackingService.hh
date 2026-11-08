#ifndef EDepSim_TrackingService_hh
#define EDepSim_TrackingService_hh 1

// EDepSimTrackingService.hh
//
// A thread-safe, re-entrant front end that wraps and manages a TrackingWorker
// running on its own dedicated std::thread.  Client code may call a Service from
// any thread(s); the TrackingChannel serializes the work.  Geant4's thread
// affinity is honored because the worker is constructed, used and destroyed
// entirely on the one thread the Service starts (see EDepSimTrackingWorker.hh).
//
// Threading modes (see the ThreadFactory and the Adopt constructor):
//   * default -- a plain std::thread runs the worker.
//   * pinned  -- the worker thread is bound to one CPU (Linux).
//   * adopt   -- no thread is created; a caller donates its own thread by
//                calling serve().  Handy under gdb/valgrind and for a CLI:
//                same code path, zero extra threading, breakpoints land where
//                you expect.

#include "EDepSimTrackingChannel.hh"
#include "EDepSimTrackingMessages.hh"

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace EDepSim {

    class TrackingService {
    public:
        /// Given the worker body, return the std::thread that will run it.
        using ThreadFactory = std::function<std::thread(std::function<void()>)>;

        static ThreadFactory DefaultFactory();       ///< plain std::thread
        static ThreadFactory PinnedFactory(int cpu); ///< bound to one CPU (Linux)

        /// Tag type selecting the adopt (no-thread) construction mode.
        struct AdoptTag {};
        static constexpr AdoptTag Adopt{};

        /// Start the worker on a dedicated thread from the factory.
        explicit TrackingService(ThreadFactory tf = DefaultFactory());

        /// Adopt mode: create no thread.  The worker runs only when a caller
        /// (typically a different thread than the one submitting work) calls
        /// serve().
        explicit TrackingService(AdoptTag);

        ~TrackingService();

        TrackingService(const TrackingService&) = delete;
        TrackingService& operator=(const TrackingService&) = delete;

        // ----------------------------------------------------------- primitives

        /// Hand a request to the worker.  Returns once the worker accepts it.
        void submit(Request req, Completion done);

        /// Blocking call built on submit; rethrows any worker-side exception.
        Reply call(Request req);

        /// Ask the worker to stop (also done by the destructor).
        void shutdown();

        // ------------------------------------------------------------- adopt

        /// Run the worker loop on the CALLING thread until shutdown/close.  Only
        /// valid for a Service constructed with Adopt.  All Geant4 work then
        /// happens on the calling thread.
        void serve();

        // -------------------------------------------------------- convenience

        /// Default mode: the worker installs its built-in generator; drive it
        /// with simulate(container, id).
        void initialize(std::string physicsList,
                        std::string gdml,
                        std::vector<std::string> macros = {},
                        bool validateGeometry = false);

        /// Custom-generator mode: the worker calls `generatorFactory` ON THE
        /// GEANT4 THREAD to build the primary generator (see GeneratorFactory --
        /// this is what preserves thread affinity), then drive it with the
        /// no-container simulate(id).  A factory returning an
        /// EDepSim::PrimaryGenerator gives access to the full VKinematicsGenerator
        /// machinery (RooTracker/HEPEVT/GPS, multi-vertex, ...).
        void initialize(std::string physicsList,
                        std::string gdml,
                        GeneratorFactory generatorFactory,
                        std::vector<std::string> macros = {},
                        bool validateGeometry = false);

        void applyMacros(std::vector<std::string> commands);

        /// Blocking one-event simulate feeding a primary-vertex container
        /// (default mode).
        std::shared_ptr<TG4Event>
        simulate(std::shared_ptr<const TG4PrimaryVertexContainer> primaries,
                 unsigned long eventId);

        /// Blocking one-event simulate with no container: the installed custom
        /// generator supplies the primaries (custom-generator mode).
        std::shared_ptr<TG4Event> simulate(unsigned long eventId);

        /// Non-blocking simulate.  The callback runs on the worker thread with
        /// either the event or an exception.  This is what a TBB async_node body
        /// calls: it must not block, or it burns a worker.
        void simulateAsync(
            std::shared_ptr<const TG4PrimaryVertexContainer> primaries,
            unsigned long eventId,
            std::function<void(std::shared_ptr<TG4Event>, std::exception_ptr)> cb);

    private:
        void threadBody();

        TrackingChannel fChannel;  ///< declared before fThread on purpose
        std::thread fThread;
        bool fAdopted{false};
    };

} // namespace EDepSim
#endif
