#ifndef EDepSim_UserStackingAction_hh_seen
#define EDepSim_UserStackingAction_hh_seen

#include "G4Track.hh"
#include "G4UserStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"

/// Control which particles are actually tracked by G4.
namespace EDepSim {class UserStackingAction;}
class EDepSim::UserStackingAction : public G4UserStackingAction {
public:
    UserStackingAction();
    virtual ~UserStackingAction();

    /// Check if a new track should be tracked.
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);

    /// Set a flag to kill optical photons.  They should be killed if there
    /// aren't any HitSurface sensitive detectors.  This is used by SurfaceSD
    /// to make sure photons are tracked.
    void SetKillOpticalPhotons(bool kill) {fKillOpticalPhotons = kill;}

    /// Check if optical photons should be killed
    bool GetKillOpticalPhotons() {return fKillOpticalPhotons;}

private:

    bool fKillOpticalPhotons;

};
#endif
