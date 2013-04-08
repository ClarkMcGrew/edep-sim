////////////////////////////////////////////////////////////
// $Id: DSimUserTrackingAction.hh,v 1.3 2004/03/18 20:49:27 t2k Exp $
//
#ifndef DSimUserTrackingAction_h
#define DSimUserTrackingAction_h 1

#include "G4UserTrackingAction.hh"
class G4Track;

class DSimUserTrackingAction : public G4UserTrackingAction
{
  public:
    DSimUserTrackingAction();
    virtual ~DSimUserTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
};
#endif
