#ifndef EDepSim_TrackingChannel_hh
#define EDepSim_TrackingChannel_hh 1

// EDepSimTrackingChannel.hh
//
// A depth-1 request/reply rendezvous slot: a mutex-protected single Message
// with two condition variables.  The worker is serial and callers block in
// send() until the worker picks up; the wakeup order among competing callers is
// unspecified.

#include "EDepSimTrackingMessages.hh"

#include <condition_variable>
#include <mutex>
#include <optional>

namespace EDepSim {

    class TrackingChannel {
    public:
        /// Service side.  Blocks until the slot is free, then hands over the
        /// message.  Throws if the channel has been closed.
        void send(Message m);

        /// Worker side.  Blocks until a message is available; returns nullopt
        /// once the channel is closed and empty ("stop looping").
        std::optional<Message> recv();

        /// Wake every waiter; subsequent send() throws and recv() returns
        /// nullopt.
        void close();

    private:
        std::mutex fMutex;
        std::condition_variable fReady;   ///< signalled when a message arrives
        std::condition_variable fFree;    ///< signalled when the slot empties
        std::optional<Message> fSlot;
        bool fClosed{false};
    };

} // namespace EDepSim
#endif
