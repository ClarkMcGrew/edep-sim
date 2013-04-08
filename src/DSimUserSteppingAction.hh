////////////////////////////////////////////////////////////
// $Id: DSimUserSteppingAction.hh,v 1.4 2004/12/04 17:33:13 jnurep Exp $
//

#ifndef DSimUserSteppingAction_h
#define DSimUserSteppingAction_h 1

#include <G4UserSteppingAction.hh>

/// An action called for each step to verify that the MC isn't caught in some
/// loop and that the particle is still near to the detector.  GEANT has a few
/// strange geometry glitchs where a very low energy photon or electron can be
/// caught by in a corner of an object.  The worst case is a low energy
/// electron in a magnetic field.  This isn't exactly a bug, since GEANT is
/// basically required to take microscopic steps and the round off causes the
/// particle to be stuck in one place.
class DSimUserSteppingAction : public G4UserSteppingAction {
public:
    DSimUserSteppingAction();
    virtual ~DSimUserSteppingAction() {};
    
    void UserSteppingAction(const G4Step*);
    
public:
    /// A count of the number of bad steps.
    int fStenchAndRot;
};

#endif
