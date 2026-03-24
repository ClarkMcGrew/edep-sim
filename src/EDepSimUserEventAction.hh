////////////////////////////////////////////////////////////
// $Id: EDepSim::UserEventAction.hh,v 1.4 2006/03/04 22:53:23 jnurep Exp $
//
#ifndef EDepSim_UserEventAction_h
#define EDepSim_UserEventAction_h 1

#include <G4UserEventAction.hh>

#include <vector>

class G4Event;

namespace EDepSim {class UserEventAction;}
class EDepSim::UserEventAction : public G4UserEventAction {
public:
    UserEventAction();
    virtual ~UserEventAction();

    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);

    /// Add an external user action to be called after the EDepSim
    /// pre and post actions.  The external action can collect information
    /// about the event, but must not modify the state of G4, or EDepSim.
    void AddExternalAction(G4UserEventAction* action) {
        fExternalActions.push_back(action);
    }

private:

    // A list of external event actions that will be called.
    std::vector<G4UserEventAction*> fExternalActions;

};

#endif
