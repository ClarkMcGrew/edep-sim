#ifndef EDepSim_Quanta_h
#define EDepSim_Quanta_h

#include "globals.hh"
#include "templates.hh"
#include "Randomize.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleMomentum.hh"
#include "G4Step.hh"
#include "G4VRestDiscreteProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4EmSaturation.hh"

/// Implement the Doke-Birk recombination probability found in NEST in an
/// optimized form for a LAr.  The Doke-Birk parameterization is quite
/// accurate for LAr over the important energy ranges for a LArTPC (E more
/// than about an MeV).
///
/// When this is called for a material other than LAr, then the G4 Birk's Law
/// implementation (G4EmSaturation) is used.
///
/// This file should always be accompanied by the full NEST implementation
/// since this is a direct modification of the NEST code.  The NEST code needs
/// to be provided since it is the arbitor of the performance of this code.
/// Where this code deviates from NEST, NEST is right.  Finally, if this is
/// used, the NEST authors should be cited.  They did all of the physics.
/// This is just an adaptation of their work to LAr so it's faster, but with
/// much less capability.
namespace EDepSim {class DokeBirks;}
class EDepSim::DokeBirks : public G4VRestDiscreteProcess //class definition
{
public:

    DokeBirks(const G4String& processName = "Doke-Birks",
                  G4ProcessType type = fElectromagnetic);
    ~DokeBirks();

    /// Determine which particles this process should be applied too.
    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
    
    /// Don't limit the step since this process doesn't actually change the
    /// energy deposit (it returns infinity).  The process does set the
    /// 'StronglyForced' condition so the DoIt is invoked at every step.
    G4double GetMeanFreePath(const G4Track& aTrack,
                             G4double ,
                             G4ForceCondition* );

    /// Don't limit the step since this process doesn't actually change the
    /// energy deposit (it returns infinity).  The process does set the
    /// 'StronglyForced' condition so the DoIt is invoked at every step.
    G4double GetMeanLifeTime(const G4Track& aTrack,
                             G4ForceCondition* );
  
    /// Apply the scintilation process for an in-flight particle.
    G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                    const G4Step& aStep);

    /// Apply the scintilation process to a stopped particle (this just calles
    /// PostStepDoIt().
    G4VParticleChange* AtRestDoIt ( const G4Track& aTrack,
                                    const G4Step& aStep);
  
private:

    /// Get the expected electron electron dEdX as a function of energy.  This
    /// is normalized to denstity.  The energy must be in MeV.
    G4double CalculateElectronLET ( G4double E);

    // The EM Saturation model for when the material is not LAr.
    G4EmSaturation* fEmSaturation;
  
};
#endif
