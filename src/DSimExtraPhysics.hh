#ifndef DSimExtraPhysics_hh_seen
#define DSimExtraPhysics_hh_seen
////////////////////////////////////////////////////////////
// $Id: DSimExtraPhysics.hh,v 1.1 2007/01/19 20:35:22 mcgrew Exp $
////////////////////////////////////////////////////////////

#include "globals.hh"

#include "G4VPhysicsConstructor.hh"

/// A G4VPhysicsConstructor to provide extra physics processes required by
/// detSim such as step length limiters.  The extra physics processes must be
/// explicitly added to every physics list (as an EM list) that is created in
/// DSimPhysicsList::AddPhysicsList(); however, this only effects code
/// actually in that method.  This class should not be used outside of
/// DSimPhysicsList.  Once this is defined, the step length can be limited in
/// logical volumes as they are created by calling
/// G4LogicalVolume::SetUsersLimits(new G4UserLimits(stepLength)).
class DSimExtraPhysics: public G4VPhysicsConstructor {
public:

    /// Construct the extra physics lists.  The argument is the default
    /// recombination for argon.  If the value is negative, then use NEST.
    explicit DSimExtraPhysics();
    virtual ~DSimExtraPhysics();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

    /// Set the ionization model to be use.  The ionization model calculates
    /// the amount of total deposited energy that will be visible as
    /// ionization.  Because of how G4 works, the total energy deposited and
    /// the non-ionization energy are tabulated.  The implemented models are:
    /// 0) Use NEST to make a detailed calculation.  1) Use
    /// DSimSimpleScintillation to make a quick calculation.
    void SetIonizationModel(int m) {fIonizationModel=m;}
    
private:

    int fIonizationModel;
};
#endif
