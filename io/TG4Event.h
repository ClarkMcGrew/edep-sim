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

    /// A container of primary vertices (a vector).  Sometimes, there is only
    /// one element, but you need to be careful since some neutrino events
    /// don't have any primary particles (e.g. neutral current events where
    /// the scattered nucleon is absorbed by the nucleus).  It's also likely
    /// that an event will have multiple vertices (i.e. more than one
    /// interaction per spill).  When no primary was provided, then a fake
    /// primary vertex will be inserted so that an empty event is generated.
    TG4PrimaryVertexContainer Primaries;

    /// A container of G4 trajectories (a vector).
    ///
    /// \note When trajectories are eliminated (e.g. very low energy electron
    /// trajectories) the TG4Trajectory::TrackId fields are adjusted so
    /// that (Trajectories[index].TrackId==index).
    TG4TrajectoryContainer Trajectories;

    /// A map of sensitive detector names and vectors of hit segments.  The
    /// map is keyed using the sensitive volume name.
    TG4HitSegmentDetectors SegmentDetectors;

    ClassDef(TG4Event,1)
};
#endif
