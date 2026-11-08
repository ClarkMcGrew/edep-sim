# ctest — CTest-based tests

CTest tests for the `EDepSim::TrackingService` facility (see
[`../doc/TrackingService.md`](../doc/TrackingService.md)).  Unlike the
script-based checks under `validate/`, these are C++ programs registered with
`add_test`, built as part of the normal CMake build and run with `ctest`.

| Test | Program | Geant4 | Checks |
|------|---------|:------:|--------|
| `TrackingChannel`      | `EDepSimTrackingChannelTest.cc`      | no  | depth-1 request round-trip, exception-as-`Reply.error`, `close()` semantics |
| `TrackingServiceError` | `EDepSimTrackingServiceErrorTest.cc` | no  | a throwing request is translated across the worker thread and rethrown to the caller |
| `TrackingServiceSim`   | `EDepSimTrackingServiceSimTest.cc`   | yes | init from GDML → simulate a muon → populated `TG4Event`; second event; clean teardown |
| `TrackingServiceAdopt` | `EDepSimTrackingServiceAdoptTest.cc` | yes | adopt mode: Geant4 on the calling thread while a driver thread submits |
| `TrackingServiceGenerator` | `EDepSimTrackingServiceGeneratorTest.cc` | yes | custom generator via a factory; asserts it was constructed on the Geant4 thread (affinity), then `simulate(id)` drives it |

`EDepSimTrackingTestPrimaries.hh` is a shared helper that builds a one-muon
`TG4PrimaryVertexContainer`.

## Running

Build edep-sim normally, then from the build directory:

```bash
ctest --output-on-failure -R Tracking
```

The two Geant4 tests need the `G4*DATA` dataset environment set (as for any
edep-sim run), and they use `inputs/example.gdml` as the geometry.  Each of them
performs a single `initialize()` because only one `G4RunManager` may exist per
process (it is leaked at shutdown, not deleted) — which is why they are separate
executables.
