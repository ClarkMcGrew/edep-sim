////////////////////////////////////////////////////////////
// $Id: EDepSim::UserEventAction.hh,v 1.4 2006/03/04 22:53:23 jnurep Exp $
//
#ifndef EDepSim_UserEventAction_h
#define EDepSim_UserEventAction_h 1

#include <G4UserEventAction.hh>

class G4Event;

namespace EDepSim {class UserEventAction;}
class EDepSim::UserEventAction : public G4UserEventAction {
public:
    UserEventAction();
    virtual ~UserEventAction();
    
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    
};

#endif

    
