#ifndef EDepSim_Quanta_h
#define EDepSim_Quanta_h

#include <globals.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4DynamicParticle.hh>
#include <G4VRestDiscreteProcess.hh>

/// Collect the secondary energy deposition part of the energy loss for the
/// EDepSim::HitSegment.  For EDepSim, secondary loss is the energy that
/// generally goes into scintillation.  The ionization energy loss is then the
/// total energy minus the secondary.
namespace EDepSim {class SecondaryEnergy;}
class EDepSim::SecondaryEnergy : public G4VRestDiscreteProcess
{
public:

    SecondaryEnergy(const G4String& processName = "Secondary-Energy",
                    G4ProcessType type = fElectromagnetic);
    ~SecondaryEnergy();

    /// Determine the particles where this process should be applied.
    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);

    /// Don't limit the step since this process doesn't actually change the
    /// energy deposit (it returns infinity).  The process does set the
    /// 'StronglyForced' condition so the DoIt is invoked at every step.
    G4double GetMeanFreePath(const G4Track& aTrack,
                             G4double dummy,
                             G4ForceCondition* force);

    /// Don't limit the step since this process doesn't actually change the
    /// energy deposit (it returns infinity).  The process does set the
    /// 'StronglyForced' condition so the DoIt is invoked at every step.
    G4double GetMeanLifeTime(const G4Track& aTrack,
                             G4ForceCondition* force);

    /// Apply the process for an in-flight particle.
    G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                    const G4Step& aStep);

    /// Apply the process to a stopped particle
    G4VParticleChange* AtRestDoIt (const G4Track& aTrack,
                                   const G4Step& aStep);

};
#endif
