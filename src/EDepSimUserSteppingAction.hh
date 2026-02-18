////////////////////////////////////////////////////////////
// $Id$
//

#ifndef EDepSimUserSteppingAction_h
#define EDepSimUserSteppingAction_h 1

#include <G4UserSteppingAction.hh>

/// An action called for each step to make sure the MC isn't caught in some
/// loop, the particle is still near to the detector, and the stepping has not
/// gone generally wacko.  GEANT can have geometry glitchs where a
/// very low energy photon or electron can be caught by in a corner of an
/// object.  The worst case is a low energy electron in a magnetic field.
/// This isn't exactly a bug, since GEANT is basically required to take
/// microscopic steps and the round off causes the particle to be stuck in one
/// place.  It can also forget to apply energy loss to silly low energy
/// particles.  This keeps things from getting caught.
namespace EDepSim {class SteppingAction;}
class EDepSim::SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction();
    virtual ~SteppingAction() {};

    void UserSteppingAction(const G4Step*);

private:
    /// A count of the number of bad steps.
    int fStenchAndRot;

    /// The number of steps for this trajectory.
    int fSteps;

    /// Control how often things are printed
    int fThrottle;

    /// Control a summary of steps for the user.
    int fGovernor;
};

#endif
