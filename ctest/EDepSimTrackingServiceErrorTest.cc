// EDepSimTrackingServiceErrorTest.cc
//
// Negative test (task ddm-2ha.4.2): a request that throws inside the worker is
// caught there and delivered as Reply.error, then rethrown to the caller by
// TrackingService::call().  This exercises the exception-translation path that
// is the concrete fix over edep-sim-phlex's Tracking (which caught only init
// and would std::terminate on a per-event throw).
//
// This program deliberately does NOT initialize Geant4 (no run manager is
// created), so it is safe to run alongside the G4 integration tests.

#include "EDepSimTrackingService.hh"

#include "TG4PrimaryVertex.h"

#include <cstdio>
#include <exception>
#include <memory>
#include <stdexcept>

int main() {
    int failures = 0;
    auto check = [&](bool ok, const char* what) {
        std::printf("%s %s\n", ok ? "ok:  " : "FAIL:", what);
        if (!ok) ++failures;
    };

    EDepSim::TrackingService svc;   // worker thread runs, but never initialized

    // simulate() before initialize() -> worker throws std::runtime_error, which
    // must surface as a C++ exception on THIS thread (not terminate the worker).
    {
        bool threw = false;
        try {
            auto primaries = std::make_shared<TG4PrimaryVertexContainer>();
            svc.simulate(primaries, 1);
        }
        catch (const std::exception& e) {
            threw = true;
            std::printf("     caught (expected): %s\n", e.what());
        }
        check(threw, "simulate-before-initialize is delivered to the caller");
    }

    // The service is still alive and responsive after a translated error.
    {
        bool threwAgain = false;
        try {
            auto primaries = std::make_shared<TG4PrimaryVertexContainer>();
            svc.simulate(primaries, 2);
        }
        catch (const std::exception&) {
            threwAgain = true;
        }
        check(threwAgain, "service still serves (and still errors) after an error");
    }

    if (failures) {
        std::printf("\n%d FAILURE(S)\n", failures);
        return 1;
    }
    std::printf("\nTrackingService error-translation test passed.\n");
    return 0;
}
