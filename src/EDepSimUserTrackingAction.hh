////////////////////////////////////////////////////////////
// $Id: EDepSim::UserTrackingAction.hh,v 1.3 2004/03/18 20:49:27 t2k Exp $
//
#ifndef EDepSim_UserTrackingAction_h
#define EDepSim_UserTrackingAction_h 1

#include <G4UserTrackingAction.hh>
class G4Track;
class G4OpBoundaryProcess;

namespace EDepSim {class UserTrackingAction;}
class EDepSim::UserTrackingAction : public G4UserTrackingAction
{
  public:
    UserTrackingAction();
    virtual ~UserTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
    // Optical photons are harvested by the UserTrackingAction, and there
    // doesn't seem to be any better way to check if the photon was detected
    // than to check the status of the G4OpBoundaryProcess.  Unfortunately,
    // that process never causes the track to stop (it's actually a
    // transportation step), so we need to check the process directly.  That
    // means keeping a cache of the opticalphoton process (when the process is
    // defined).
    bool fOpticalPhotonOpBoundaryProcessCached;  // True if looked for
    const G4OpBoundaryProcess* fOpticalPhotonOpBoundaryProcess;

};
#endif
