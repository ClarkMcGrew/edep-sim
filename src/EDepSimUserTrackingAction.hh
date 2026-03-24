////////////////////////////////////////////////////////////
// $Id: EDepSim::UserTrackingAction.hh,v 1.3 2004/03/18 20:49:27 t2k Exp $
//
#ifndef EDepSim_UserTrackingAction_h
#define EDepSim_UserTrackingAction_h 1

#include <G4UserTrackingAction.hh>

#include <vector>

class G4Track;
class G4OpBoundaryProcess;

namespace EDepSim {class UserTrackingAction;}
class EDepSim::UserTrackingAction : public G4UserTrackingAction
{
  public:
    UserTrackingAction();
    virtual ~UserTrackingAction();

    /// The action called by G4 to notify the user before a track is started.
    virtual void PreUserTrackingAction(const G4Track*);

    /// The action called by G4 to notify the user after a track is finished.
    virtual void PostUserTrackingAction(const G4Track*);

    /// Add an external user tracking action to be called before the EDepSim
    /// pre and post actions.  The external action can collect information
    /// about the track, but must not modify the state of G4, or EDepSim.
    void AddExternalAction(G4UserTrackingAction* action) {
        fExternalActions.push_back(action);
    }

    /// If this is true, then do not create trajectories for opticalphotons.
    /// This makes the stacking and tracking a bit faster and uses less
    /// memory.
    void SetSavePhotonTrajectories(bool s) {fSavePhotonTrajectories = true;}
    bool GetSavePhotonTrajectories() const {return fSavePhotonTrajectories;}

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

    // If true, then save the opticalphoton trajectories.
    bool fSavePhotonTrajectories;

    // A list of external tracking actions that will be called.
    std::vector<G4UserTrackingAction*> fExternalActions;
};
#endif
