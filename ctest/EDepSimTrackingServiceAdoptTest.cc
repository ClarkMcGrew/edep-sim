// EDepSimTrackingServiceAdoptTest.cc
//
// Adopt-mode test (task ddm-2ha.4.3): no worker thread is created; a driver
// thread submits work while the MAIN thread runs the worker via serve().  All
// Geant4 work therefore happens on the calling (main) thread -- the mode that
// makes gdb/valgrind sessions and a plain CLI behave predictably.
//
// Usage: EDepSimTrackingServiceAdoptTest <geometry.gdml> [physics-list]

#include "EDepSimTrackingService.hh"

#include "EDepSimTrackingTestPrimaries.hh"
#include "TG4Event.h"

#include <atomic>
#include <cstdio>
#include <exception>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: %s <geometry.gdml> [physics-list]\n", argv[0]);
        return 2;
    }
    const std::string gdml = argv[1];
    const std::string physicsList = (argc > 2) ? argv[2] : std::string{};

    std::atomic<int> failures{0};
    auto check = [&](bool ok, const char* what) {
        std::printf("%s %s\n", ok ? "ok:  " : "FAIL:", what);
        if (!ok) ++failures;
    };

    EDepSim::TrackingService svc(EDepSim::TrackingService::Adopt);

    // The driver submits work from another thread; the worker itself runs on the
    // main thread inside serve() below.
    std::thread driver([&] {
        try {
            svc.initialize(physicsList, gdml);
            auto ev = svc.simulate(edepsim_ctest::one_muon(1000.0), 99);
            check(static_cast<bool>(ev), "adopt-mode simulate returned a TG4Event");
            if (ev) {
                std::printf("     primaries=%zu trajectories=%zu eventId=%d\n",
                            ev->Primaries.size(), ev->Trajectories.size(),
                            ev->EventId);
                check(ev->Trajectories.size() >= 1, "adopt-mode event was tracked");
                check(ev->EventId == 99, "adopt-mode EventId stamped");
            }
        }
        catch (const std::exception& e) {
            std::printf("FAIL: driver threw: %s\n", e.what());
            ++failures;
        }
        svc.shutdown();   // makes serve() return on the main thread
    });

    svc.serve();          // runs Geant4 on THIS (main) thread until shutdown
    driver.join();

    if (failures) {
        std::printf("\n%d FAILURE(S)\n", failures.load());
        return 1;
    }
    std::printf("\nTrackingService adopt-mode test passed.\n");
    return 0;
}
