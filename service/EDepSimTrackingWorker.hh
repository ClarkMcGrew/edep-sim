#ifndef EDepSim_TrackingWorker_hh
#define EDepSim_TrackingWorker_hh 1

// EDepSimTrackingWorker.hh
//
// Everything Geant4 lives here.  A TrackingWorker MUST be constructed, used AND
// destroyed on one dedicated std::thread: Geant4's sequential G4RunManager is
// thread-affine (its navigator/world live in G4ThreadLocal state) and the
// global geometry stores are torn down by main-thread static destructors at
// process exit.  So construction, every beamOn, and the final teardown all run
// on the single thread that TrackingService gives this object.
//
// The Geant4 objects live behind a pimpl (struct Impl) so this header stays
// Geant4-free.  doShutdown() empties the global geometry stores and then
// destroys the Geant4 objects, all on this thread (see the .cc for why the
// store cleanup must precede -- and enables -- the run-manager destruction).

#include "EDepSimTrackingMessages.hh"

#include <memory>

namespace EDepSim {

    class TrackingChannel;

    class TrackingWorker {
    public:
        TrackingWorker();
        ~TrackingWorker();

        TrackingWorker(const TrackingWorker&) = delete;
        TrackingWorker& operator=(const TrackingWorker&) = delete;

        /// Service one message at a time until a ShutdownRequest is received or
        /// the channel closes, then run the teardown discipline.  Every request
        /// is serviced inside a try/catch so a failure is returned as a reply
        /// error rather than terminating the process.
        void run(TrackingChannel& channel);

    private:
        void doInitialize(const InitializeRequest& req);
        void doMacros(const ApplyMacroRequest& req);
        std::shared_ptr<TG4Event> doSimulate(const SimulateRequest& req);
        void doShutdown();

        struct Impl;                    ///< holds the Geant4 objects
        std::unique_ptr<Impl> fImpl;
        bool fInitialized{false};
    };

} // namespace EDepSim
#endif
