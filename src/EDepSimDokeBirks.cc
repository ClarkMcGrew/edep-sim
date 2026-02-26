#include <G4ParticleTypes.hh> //lets you refer to G4OpticalPhoton, etc.
#include <G4EmProcessSubType.hh> //lets you call this process Scintillation
#include <G4Version.hh> //tells you what Geant4 version you are running
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>
#include <G4FieldManager.hh>
#include <G4Field.hh>
#include <G4EmParameters.hh>

#include "EDepSimLog.hh"

#include "EDepSimDokeBirks.hh"

// The physics in EDepSim::DokeBirks is shamelessly stolen from NEST, so cite
// the NEST paper to give credit where credit is due.  However, all of the
// bugs are mine, so there should be a note like "Simplified implementation of
// the physics described in NEST paper".  This depends on DokeBirksSaturation
// for the physics.
EDepSim::DokeBirks::DokeBirks(const G4String& processName,
                             G4ProcessType type)
    : G4VRestDiscreteProcess(processName, type)
{
    SetProcessSubType(fScintillation);

    if (verboseLevel>0) {
        G4cout << GetProcessName() << " is created " << G4endl;
    }

}

EDepSim::DokeBirks::~DokeBirks () {} //destructor needed to avoid linker error


G4bool
EDepSim::DokeBirks::IsApplicable(const G4ParticleDefinition& aParticleType)
{
    // This process is applicable to some neutral particles (e.g. gammas)
    // since G4 will fake creating the low energy electrons.
    if (aParticleType.IsShortLived()) return false;
    if (aParticleType.GetParticleName() == "opticalphoton") return false;
    if (aParticleType.GetParticleName() == "thermalelectron") return false;
    // Not applicable to the neutrinos.
    if (std::abs(aParticleType.GetPDGEncoding())==12) return false;
    if (std::abs(aParticleType.GetPDGEncoding())==14) return false;
    if (std::abs(aParticleType.GetPDGEncoding())==16) return false;
    return true;
}

// This routine simply calls the equivalent PostStepDoIt since all the
// necessary information resides in aStep.GetTotalEnergyDeposit()
G4VParticleChange*
EDepSim::DokeBirks::AtRestDoIt(const G4Track& aTrack, const G4Step& aStep)
{
    return EDepSim::DokeBirks::PostStepDoIt(aTrack, aStep);
}

// this is the most important function, where all light & charge yields
// happen!
G4VParticleChange*
EDepSim::DokeBirks::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
    aParticleChange.Initialize(aTrack);

    const G4Material* aMaterial = aTrack.GetMaterial();
    const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
    const G4ParticleDefinition *pDef = aParticle->GetDefinition();
    G4String particleName = pDef->GetParticleName();
    G4double totalEnergyDeposit = aStep.GetTotalEnergyDeposit();

    if (totalEnergyDeposit <= 0.0) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    if (aStep.GetStepLength() <= 0.0) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

#if G4VERSION_NUMBER >= 1100
    // The G4 EMSaturation model is update to expect a charged particle.
    if (std::abs(aParticle->GetCharge()) < 0.1) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
#endif

    G4double nonIonizingEnergy
        = G4EmParameters::Instance()->GetEmSaturation()
        ->VisibleEnergyDepositionAtAStep(&aStep);
    aParticleChange.ProposeNonIonizingEnergyDeposit(nonIonizingEnergy);

    EDepSimTrace("Nonionizing " << nonIonizingEnergy
                 << " Total " << totalEnergyDeposit
                 << " Ratio " << nonIonizingEnergy/totalEnergyDeposit
                 << " for " << particleName
                 << " in " << aMaterial->GetName());

    return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

// GetMeanFreePath
// ---------------
G4double EDepSim::DokeBirks::GetMeanFreePath(const G4Track&,
                                     G4double ,
                                     G4ForceCondition* condition)
{
    *condition = StronglyForced;
    // Force the EDepSim::DokeBirks physics process to always happen, even if
    // it's not the dominant process.  In effect it is a meta-process on top
    // of any and all other energy depositions which may occur (disregard
    // DBL_MAX, this is "StronglyForced" so it makes the mean free path zero
    // really, not infinite)

    return DBL_MAX;
}

// GetMeanLifeTime
// ---------------
G4double EDepSim::DokeBirks::GetMeanLifeTime(const G4Track&,
                                     G4ForceCondition* condition)
{
    *condition = Forced;
    // This function and this condition has the same effect as the above

    return DBL_MAX;
}
