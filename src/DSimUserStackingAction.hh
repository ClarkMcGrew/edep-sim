#ifndef DSimUserStackingAction_hh_seen
#define DSimUserStackingAction_hh_seen

#include "G4Track.hh"
#include "G4UserStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"

/// Control which particles are actually tracked by G4.  
class DSimUserStackingAction : public G4UserStackingAction {
public:
    DSimUserStackingAction();
    virtual ~DSimUserStackingAction();

    /// Check if a new track should be tracked.
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
};
#endif
