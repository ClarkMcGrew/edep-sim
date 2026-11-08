// EDepSimTrackingServiceSimTest.cc
//
// Integration test (task ddm-2ha.4.1): drive one Geant4 event end-to-end
// through a default (threaded) TrackingService and check the returned TG4Event,
// then let the service destruct to exercise the on-worker-thread teardown.
//
// Usage: EDepSimTrackingServiceSimTest <geometry.gdml> [physics-list]
//
// NOTE: exactly one Geant4 run manager may exist per process (it is leaked at
// shutdown, not deleted), so this program performs a single initialize().

#include "EDepSimTrackingService.hh"

#include "EDepSimTrackingTestPrimaries.hh"
#include "TG4Event.h"

#include <cstdio>
#include <memory>
#include <string>

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

    {
        EDepSim::TrackingService svc;   // default: worker on a dedicated thread
        svc.initialize(physicsList, gdml);

        auto ev = svc.simulate(edepsim_ctest::one_muon(1000.0), 42);

        check(static_cast<bool>(ev), "simulate returned a TG4Event");
        if (ev) {
            std::printf("     primaries=%zu trajectories=%zu segment-detectors=%zu\n",
                        ev->Primaries.size(), ev->Trajectories.size(),
                        ev->SegmentDetectors.size());
            check(ev->Primaries.size() >= 1, "TG4Event has >= 1 primary vertex");
            check(ev->Trajectories.size() >= 1, "TG4Event has >= 1 trajectory");
            check(ev->EventId == 42, "TG4Event.EventId stamped from event id");

            // A forward muon through the LAr tracker should deposit energy in a
            // sensitive detector, i.e. produce hit segments.
            std::size_t nseg = 0;
            for (const auto& kv : ev->SegmentDetectors) nseg += kv.second.size();
            std::printf("     total hit segments=%zu\n", nseg);
            check(nseg >= 1, "TG4Event has >= 1 hit segment");
        }

        // Second event on the same service: reuse the initialized worker.
        auto ev2 = svc.simulate(edepsim_ctest::one_muon(500.0), 7);
        check(ev2 && ev2->EventId == 7, "second simulate on same service works");

        // svc destructs here -> ShutdownRequest, geometry-store teardown on the
        // worker thread, join.  A clean process exit (below) validates it.
    }

    if (failures) {
        std::printf("\n%d FAILURE(S)\n", failures);
        return 1;
    }
    std::printf("\nTrackingService integration test passed; clean teardown.\n");
    return 0;
}
