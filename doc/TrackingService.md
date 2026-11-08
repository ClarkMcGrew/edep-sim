# EDepSim::TrackingService — thread-affine Geant4 lifecycle

Geant4's sequential `G4RunManager` is **thread-affine**: the navigator/world it
builds live in `G4ThreadLocal` state, and the global geometry stores are torn
down by main-thread static destructors at process exit.  So *every* Geant4
action — construction, each `beamOn`, and destruction — must happen on **one**
dedicated thread.  Building on one thread and running or destroying on another
corrupts Geant4 and crashes (typically in `~G4PVPlacement`/`GetRotation`).

`EDepSim::TrackingService` wraps that discipline behind a small, framework-
agnostic API.  It may be used to run edep-sim in an application where calls may
come from multiple, changing threads.  For examples, see Wire-Cell Toolkit's
wire-cell-edep or the Phlex framework integration package edep-sim-phlex which
uses the tracking service to protect Geant4 from TBB `flow_graph` thread model.

## Classes

| Class | Role |
|-------|------|
| `EDepSim::TrackingService` | Thread-safe, re-entrant front end owned by client code. Manages the worker's dedicated thread. |
| `EDepSim::TrackingWorker`  | Everything Geant4. Constructed, used **and destroyed** on the one dedicated thread. |
| `EDepSim::TrackingChannel` | Depth-1 request/reply rendezvous (mutex + two condition variables). |
| `EDepSim::PrimaryVertexGenerator` | `G4VPrimaryGenerator` converting `TG4PrimaryVertexContainer` → Geant4 primaries. |

Messages (`EDepSim::InitializeRequest`, `ApplyMacroRequest`, `SimulateRequest`,
`ShutdownRequest` and the `Reply` type) are in `EDepSimTrackingMessages.hh`.

A request flows from client code, through the service and worker, into Geant4,
and the result flows back the same way — crossing the thread boundary once, via
the channel:

```
  ┌───────────────┐
  │  client code  │   any thread; re-entrant
  └───────┬───────┘
          │  submit(request)  ·  the call blocks for the reply
          ▼
  ┌───────┴───────┐
  │ TrackingSvc   │  thread-safe front end (owns channel + worker thread)
  └───────┬───────┘
          │  Message  ─────────────────────────►  via TrackingChannel
          ▼           (the reply travels back up)  depth-1 rendezvous
  ════════╪═══════════ dedicated Geant4 thread ══════════════════════
          ▼
  ┌───────┴───────┐   beamOn(1)   ┌──────────────┐
  │ TrackingWkr   │ ────────────► │    Geant4    │
  │ std::visit →  │               │ G4RunManager │
  │ do_{init,sim, │ ◄──────────── │  → TG4Event  │
  │     shutdown} │   TG4Event    └──────────────┘
  └───────────────┘
```

## Basic use

```cpp
#include "EDepSimTrackingService.hh"

EDepSim::TrackingService svc;                  // worker on a dedicated thread
svc.initialize(/*physicsList=*/"", "geom.gdml",
               /*macros=*/{"/edep/db/set/neutronThreshold 100 MeV"});

auto event = svc.simulate(primaries, /*event_id=*/42);  // -> std::shared_ptr<TG4Event>
// event_id also seeds the RNG, so a given id reproduces a given event.

// svc's destructor sends a ShutdownRequest, the worker empties the Geant4
// geometry stores on its own thread, and the thread is joined.
```

Each `simulate` call is a single round trip: the container and event id go in,
one Geant4 event runs, and the summary comes back:

```
  ┌────────┐   simulate(primaries, id)   ┌──────────────────┐   beamOn(1)
  │ client │ ──────────────────────────► │ TrackingService  │ ───────────►  Geant4
  │  code  │ ◄────────────────────────── │  (+ worker on    │ ◄───────────
  └────────┘   shared_ptr<TG4Event>      │   the G4 thread) │    TG4Event
                                         └──────────────────┘
```

`primaries` is a `std::shared_ptr<const TG4PrimaryVertexContainer>`.  Values are
interpreted in Geant4/CLHEP units (mm, ns, MeV) — the same units the edep-sim
i/o classes use (see `io/EDepSimUnits.h`), so no conversion is applied.

## Custom primary generators (and thread affinity)

The container is a convenient, dependency-light value object, but it is a
repurposed *output* type and is lossy relative to native Geant4 primaries (no
polarization, charge, proper time, or pre-assigned decays).  For full control —
including edep-sim's own kinematics machinery — supply a **generator factory**:

```cpp
svc.initialize(physicsList, gdml,
               EDepSim::GeneratorFactory([]{ return new MyGenerator(); }));
auto event = svc.simulate(/*event_id=*/42);   // no container: MyGenerator drives
```

This expands the basic round trip with your generator in the path.  First
`initialize` runs the factory **on the Geant4 thread** to build the generator
there; then each `simulate(id)` drives it:

```
  1) initialize(physicsList, gdml, GeneratorFactory)
     the factory is called ON the Geant4 thread, so your generator is built there

     ┌────────┐   GeneratorFactory   ┌──────────────────┐  factory()   ┌──────────────────┐
     │ client │ ───────────────────► │ TrackingService  │ ───────────► │  your generator  │
     └────────┘                      └──────────────────┘ (G4 thread)  │  (G4VPrimaryGen) │
                                                                       └──────────────────┘

  2) simulate(id)   — no container; your generator supplies the primaries

     ┌────────┐    simulate(id)    ┌──────────────────┐  beamOn(1)  ┌──────────────────┐
     │ client │ ─────────────────► │ TrackingService  │ ──────────► │  your generator  │
     │        │                    │                  │             │  Generate-       │
     │        │                    │                  │             │  PrimaryVertex() │
     │        │                    │                  │             │  → G4 primaries  │
     │        │                    │                  │             └────────┬─────────┘
     │        │                    │                  │                      ▼
     │        │                    │                  │                    Geant4
     │        │ ◄───────────────── │                  │ ◄───────────────  TG4Event
     └────────┘shared_ptr<TG4Event>└──────────────────┘
```

The interface takes a *factory*, and not a ready-made generator to assure the
generator is constructed on the dedicated Geant4 thread.  This done as a
`G4VPrimaryGenerator`'s constructor can touch thread-local Geant4 state
(`G4ParticleTable`, `G4UImanager`/messengers, `G4Allocator` pools); constructing
it on another thread and handing it over would bind that state to the wrong
thread and break affinity.

Because `EDepSim::PrimaryGenerator` *is* a `G4VPrimaryGenerator`, a factory may
return one — assembled from `VKinematicsGenerator` + count/position/time
sub-generators (RooTracker, HEPEVT, GPS, …) — to get the complete edep-sim input
path.  Build those sub-generators inside the factory too, so they are created on
the Geant4 thread as well.

The container path and the factory path are mutually exclusive per Service:
`simulate(container, id)` requires the default (built-in) generator, and
`simulate(id)` requires a factory-installed one.

## Threading modes

- **Default** — `TrackingService svc;` runs the worker on a plain `std::thread`.
- **Pinned** — `TrackingService svc(EDepSim::TrackingService::PinnedFactory(cpu));`
  binds the worker thread to one CPU (Linux; a no-op elsewhere).
- **Adopt** — `TrackingService svc(EDepSim::TrackingService::Adopt);` creates no
  thread.  A driver thread submits work while the main thread runs the worker
  via `svc.serve()` (returns when `svc.shutdown()` is called).  All Geant4 work
  then happens on the calling thread — ideal under gdb/valgrind and for a CLI.

## Async

`simulateAsync(primaries, id, callback)` submits without blocking; the callback
runs on the worker thread with either the `TG4Event` or a `std::exception_ptr`.
This is what a TBB `async_node` body calls (it must not block a TBB worker).

## Error handling

Every request is serviced inside a `try/catch` on the worker thread; a failure
is returned as `Reply::error` and rethrown to the caller by the blocking calls
(`initialize`, `simulate`, …) or delivered to the `simulateAsync` callback.  A
per-event failure therefore never terminates the process.

## Constraints & gotchas

- **One run manager per process.**  A process may `initialize()` **once** — use
  one long-lived Service.  (Geant4's sequential `G4RunManager` is a singleton;
  the Service does destroy it cleanly at shutdown, but on the same worker thread
  and only after emptying the global geometry stores there.)
- **Writing `TG4PrimaryVertexContainer`.**  A producer that fills the primaries
  must be compiled with `-DEDEPSIM_USE_PUBLIC_FIELDS`, because
  `TG4PrimaryParticle`'s i/o fields are private under the default layout.
- **Geant4 datasets.**  Running an event needs the `G4*DATA` environment, as for
  any edep-sim run.
