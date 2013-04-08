////////////////////////////////////////////////////////////
// $Id: DSimUserEventAction.hh,v 1.4 2006/03/04 22:53:23 jnurep Exp $
//
#ifndef DSimUserEventAction_h
#define DSimUserEventAction_h 1

#include <G4UserEventAction.hh>

class G4Event;

class DSimUserEventAction : public G4UserEventAction {
public:
    DSimUserEventAction();
    virtual ~DSimUserEventAction();
    
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    
};

#endif

    
