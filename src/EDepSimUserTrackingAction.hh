////////////////////////////////////////////////////////////
// $Id: EDepSim::UserTrackingAction.hh,v 1.3 2004/03/18 20:49:27 t2k Exp $
//
#ifndef EDepSim_UserTrackingAction_h
#define EDepSim_UserTrackingAction_h 1

#include "G4UserTrackingAction.hh"
class G4Track;

namespace EDepSim {class UserTrackingAction;}
class EDepSim::UserTrackingAction : public G4UserTrackingAction
{
  public:
    UserTrackingAction();
    virtual ~UserTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

};
#endif
