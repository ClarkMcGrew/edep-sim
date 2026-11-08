// EDepSimTrackingService.cc

#include "EDepSimTrackingService.hh"

#include "EDepSimTrackingWorker.hh"

#if defined(__linux__)
#include <pthread.h>
#include <sched.h>
#endif

#include <future>
#include <stdexcept>
#include <utility>

namespace EDepSim {

    TrackingService::ThreadFactory TrackingService::DefaultFactory() {
        return [](std::function<void()> body) {
            return std::thread(std::move(body));
        };
    }

    TrackingService::ThreadFactory TrackingService::PinnedFactory(int cpu) {
        return [cpu](std::function<void()> body) {
            return std::thread([cpu, body = std::move(body)]() {
#if defined(__linux__)
                cpu_set_t set;
                CPU_ZERO(&set);
                CPU_SET(cpu, &set);
                pthread_setaffinity_np(pthread_self(), sizeof(set), &set);
#else
                (void)cpu;
#endif
                body();
            });
        };
    }

    TrackingService::TrackingService(ThreadFactory tf)
        : fThread(tf([this] { threadBody(); })) {}

    TrackingService::TrackingService(AdoptTag) : fAdopted(true) {}

    TrackingService::~TrackingService() {
        // Best-effort stop; the worker breaks its loop and runs teardown on its
        // own thread.
        try {
            fChannel.send(Message{ShutdownRequest{}, nullptr});
        }
        catch (...) {
            // channel already closed -- nothing to do
        }
        fChannel.close();
        if (fThread.joinable()) {
            fThread.join();
        }
    }

    void TrackingService::threadBody() {
        TrackingWorker worker;   // ctor on the dedicated thread
        worker.run(fChannel);
    }

    void TrackingService::serve() {
        if (!fAdopted) {
            throw std::runtime_error(
                "EDepSim::TrackingService::serve() is only valid in adopt mode");
        }
        TrackingWorker worker;   // ctor on the calling (adopted) thread
        worker.run(fChannel);
    }

    void TrackingService::submit(Request req, Completion done) {
        fChannel.send(Message{std::move(req), std::move(done)});
    }

    Reply TrackingService::call(Request req) {
        std::promise<Reply> promise;
        auto future = promise.get_future();
        submit(std::move(req),
               [&promise](Reply r) { promise.set_value(std::move(r)); });
        Reply r = future.get();
        if (r.error) {
            std::rethrow_exception(r.error);
        }
        return r;
    }

    void TrackingService::shutdown() {
        fChannel.send(Message{ShutdownRequest{}, nullptr});
    }

    void TrackingService::initialize(std::string physicsList,
                                     std::string gdml,
                                     std::vector<std::string> macros,
                                     bool validateGeometry) {
        call(InitializeRequest{std::move(physicsList), std::move(gdml),
                               std::move(macros), validateGeometry, {}});
    }

    void TrackingService::initialize(std::string physicsList,
                                     std::string gdml,
                                     GeneratorFactory generatorFactory,
                                     std::vector<std::string> macros,
                                     bool validateGeometry) {
        call(InitializeRequest{std::move(physicsList), std::move(gdml),
                               std::move(macros), validateGeometry,
                               std::move(generatorFactory)});
    }

    void TrackingService::applyMacros(std::vector<std::string> commands) {
        call(ApplyMacroRequest{std::move(commands)});
    }

    std::shared_ptr<TG4Event> TrackingService::simulate(
        std::shared_ptr<const TG4PrimaryVertexContainer> primaries,
        unsigned long eventId) {
        Reply r = call(SimulateRequest{std::move(primaries), eventId});
        return std::get<SimulateReply>(r.value).event;
    }

    std::shared_ptr<TG4Event> TrackingService::simulate(unsigned long eventId) {
        Reply r = call(SimulateRequest{nullptr, eventId});
        return std::get<SimulateReply>(r.value).event;
    }

    void TrackingService::simulateAsync(
        std::shared_ptr<const TG4PrimaryVertexContainer> primaries,
        unsigned long eventId,
        std::function<void(std::shared_ptr<TG4Event>, std::exception_ptr)> cb) {
        submit(SimulateRequest{std::move(primaries), eventId},
               [cb = std::move(cb)](Reply r) {
                   if (r.error) {
                       cb(nullptr, r.error);
                   }
                   else {
                       cb(std::get<SimulateReply>(r.value).event, nullptr);
                   }
               });
    }

} // namespace EDepSim
