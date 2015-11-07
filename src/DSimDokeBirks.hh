#ifndef DSimQuanta_h
#define DSimQuanta_h

#include "globals.hh"
#include "templates.hh"
#include "Randomize.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleMomentum.hh"
#include "G4Step.hh"
#include "G4VRestDiscreteProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"

/// Implement the Doke-Birk recombination probability found in NEST in an
/// optimized form for a LArTPC.  The Doke-Birk parameterization is quite
/// accurate for LAr over the important energy ranges for a LArTPC (E more
/// than about an MeV).
///
/// This file should always be accompanied by
/// the full NEST implementation since this is a direct modification of the
/// NEST code.  The NEST code needs to be provided since it is the arbitor of
/// the performance of this code.  Where this code deviates from NEST, NEST is
/// right.  Finally, if this is used, the NEST authors should be cited.  They
/// did all of the physics.  This is just an adaptation of their work to LAr
/// so it's faster, but with much less capability.
class DSimDokeBirks : public G4VRestDiscreteProcess //class definition
{
    // Class inherits publicly from G4VRestDiscreteProcess
private:
public: // constructor and destructor

    DSimDokeBirks(const G4String& processName = "Doke-Birks",
                  G4ProcessType type = fElectromagnetic);
    ~DSimDokeBirks();

public: // methods, with descriptions
    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
    // Returns true -> 'is applicable', for any particle type except for an
    // 'opticalphoton' and for short-lived particles

    G4double GetMeanFreePath(const G4Track& aTrack,
                             G4double ,
                             G4ForceCondition* );
    // Returns infinity; i. e. the process does not limit the step, but 
    // sets the 'StronglyForced' condition for the DoIt to be invoked at
    // every step.

    G4double GetMeanLifeTime(const G4Track& aTrack,
                             G4ForceCondition* );
    // Returns infinity; i. e. the process does not limit the time, but
    // sets the 'StronglyForced' condition for the DoIt to be invoked at
    // every step.
  
    // For in-flight particles losing energy (or those stopped)
    G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                    const G4Step& aStep);
    G4VParticleChange* AtRestDoIt ( const G4Track& aTrack,
                                    const G4Step& aStep);
  
private:

    // The default electric field in the liquid argon
    G4double fElectricField;
  
};

////////////////////
// Inline methods
////////////////////
inline 
G4bool DSimDokeBirks::IsApplicable(const G4ParticleDefinition& aParticleType)
{
    if (aParticleType.GetParticleName() == "opticalphoton") return false;
    if (aParticleType.IsShortLived()) return false;
    if(abs(aParticleType.GetPDGCharge()) < 0.1) return false;
       
    return true;
}

#endif
