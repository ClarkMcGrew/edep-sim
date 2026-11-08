// EDepSimTrackingChannelTest.cc
//
// Standalone (Geant4-free) unit test of EDepSim::TrackingChannel and the
// request/reply message protocol.  Exercises the depth-1 rendezvous: request
// round-trip, exception-as-reply, and close() semantics.  Task ddm-2ha.1.3.
//
// Build (from the edep-sim source root), e.g.:
//   g++ -std=c++23 -I src -I io \
//       test/EDepSimTrackingChannelTest.cc src/EDepSimTrackingChannel.cc \
//       -o /tmp/channel-test -lpthread -I<root-include>
// (ROOT include is only needed because the message header pulls in TG4Event.h.)

#include "EDepSimTrackingChannel.hh"
#include "EDepSimTrackingMessages.hh"

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <thread>

using namespace EDepSim;

namespace {

    int gFailures = 0;

    void check(bool ok, const char* what) {
        if (!ok) {
            std::printf("FAIL: %s\n", what);
            ++gFailures;
        }
        else {
            std::printf("ok:   %s\n", what);
        }
    }

    // A worker-like consumer that services messages until the channel closes.
    // ApplyMacroRequest with commands=={"boom"} is used to force an exception,
    // which the consumer translates into Reply.error -- mirroring the real
    // TrackingWorker's per-request try/catch.
    void consume(TrackingChannel& chan) {
        while (auto msg = chan.recv()) {
            Reply rep;
            try {
                if (auto* m = std::get_if<ApplyMacroRequest>(&msg->request)) {
                    if (!m->commands.empty() && m->commands[0] == "boom") {
                        throw std::runtime_error("boom");
                    }
                    rep.value = Empty{};
                }
                else {
                    rep.value = Empty{};
                }
            }
            catch (...) {
                rep.error = std::current_exception();
            }
            if (msg->done) { msg->done(std::move(rep)); }
        }
    }

} // namespace

int main() {
    // --- request round-trip through the depth-1 slot -----------------------
    {
        TrackingChannel chan;
        std::thread worker([&] { consume(chan); });

        int completed = 0;
        for (int i = 0; i < 100; ++i) {
            bool got = false;
            chan.send(Message{ApplyMacroRequest{{"/ok"}},
                              [&](Reply r) {
                                  got = !r.error;
                                  ++completed;
                              }});
            // send() returns once the worker accepts; the completion runs on the
            // worker thread.  Spin briefly for it (single in-flight by design).
            while (!got) { std::this_thread::yield(); }
        }
        check(completed == 100, "100 requests each round-tripped a reply");

        chan.close();
        worker.join();
    }

    // --- exception is delivered as Reply.error, not thrown on worker thread --
    {
        TrackingChannel chan;
        std::thread worker([&] { consume(chan); });

        bool sawError = false;
        bool done = false;
        chan.send(Message{ApplyMacroRequest{{"boom"}},
                          [&](Reply r) {
                              sawError = static_cast<bool>(r.error);
                              done = true;
                          }});
        while (!done) { std::this_thread::yield(); }
        check(sawError, "throwing request delivered as Reply.error");

        chan.close();
        worker.join();
    }

    // --- close() makes recv() return nullopt and send() throw ---------------
    {
        TrackingChannel chan;
        chan.close();

        bool threw = false;
        try {
            chan.send(Message{ShutdownRequest{}, nullptr});
        }
        catch (const std::exception&) {
            threw = true;
        }
        check(threw, "send() on a closed channel throws");

        auto msg = chan.recv();
        check(!msg.has_value(), "recv() on a closed channel returns nullopt");
    }

    if (gFailures) {
        std::printf("\n%d FAILURE(S)\n", gFailures);
        return 1;
    }
    std::printf("\nAll TrackingChannel tests passed.\n");
    return 0;
}
