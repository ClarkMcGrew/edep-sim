#ifndef TG4Event_hxx_seen
#define TG4Event_hxx_seen

#include "TG4PrimaryVertex.h"
#include "TG4Trajectory.h"
#include "TG4HitSegment.h"

#include <TObject.h>

class TG4Event : public TObject {
public:
    TG4Event(void) {}
    virtual ~TG4Event();

    /// The run number
    int RunId;
    
    /// The event number
    int EventId;

    /// A container of primary vertices.  Normally, there is only one element,
    /// but you need to be careful since some neutrino events don't have any
    /// primary particles (e.g. neutral current events where the scattered
    /// nucleon is absorbed by the nucleus).  It's also possible that an event
    /// could have multiple vertices (i.e. more than one interaction per
    /// spill).
    std::vector<TG4PrimaryVertex> Primaries;

    /// The trajectories
    TG4TrajectoryContainer Trajectories;

    /// The hit segments.
    TG4HitSegmentDetectors SegmentDetectors;

    ClassDef(TG4Event,1)
};
#endif
