// EDepSimTrackingChannel.cc

#include "EDepSimTrackingChannel.hh"

#include <stdexcept>
#include <utility>

namespace EDepSim {

    void TrackingChannel::send(Message m) {
        std::unique_lock<std::mutex> lk(fMutex);
        fFree.wait(lk, [this] { return !fSlot || fClosed; });
        if (fClosed) {
            throw std::runtime_error("EDepSim::TrackingChannel is closed");
        }
        fSlot = std::move(m);
        fReady.notify_one();
    }

    std::optional<Message> TrackingChannel::recv() {
        std::unique_lock<std::mutex> lk(fMutex);
        fReady.wait(lk, [this] { return fSlot.has_value() || fClosed; });
        if (!fSlot) {
            return std::nullopt;   // closed and empty
        }
        Message m = std::move(*fSlot);
        fSlot.reset();
        fFree.notify_one();
        return m;
    }

    void TrackingChannel::close() {
        {
            std::lock_guard<std::mutex> lk(fMutex);
            fClosed = true;
        }
        fReady.notify_all();
        fFree.notify_all();
    }

} // namespace EDepSim
