#include <G4Version.hh>

#include <G4ParticleTypes.hh>
#include <G4EmProcessSubType.hh>
#include <G4EmParameters.hh>
#include <G4EmSaturation.hh>

#include "EDepSimLog.hh"
#include "EDepSimSecondaryEnergy.hh"

// Keep track of the secondary energy deposit for EDepSim::HitSegment.  This
// depends on DokeBirksSaturation for the physics, and that is enabled in
// EDepSimPhysicsList by setting the EmSaturation class in G4EmParameters.
EDepSim::SecondaryEnergy::SecondaryEnergy(const G4String& processName,
                                          G4ProcessType type)
    : G4VRestDiscreteProcess(processName, type)
{
    SetProcessSubType(fScintillation);

    if (verboseLevel>0) {
        G4cout << GetProcessName() << " is created " << G4endl;
    }
}

EDepSim::SecondaryEnergy::~SecondaryEnergy () {}

G4bool
EDepSim::SecondaryEnergy::IsApplicable(
    const G4ParticleDefinition& aParticleType)
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

// This routine simply calls the equivalent PostStepDoIt.
G4VParticleChange*
EDepSim::SecondaryEnergy::AtRestDoIt(const G4Track& aTrack, const G4Step& aStep)
{
    return EDepSim::SecondaryEnergy::PostStepDoIt(aTrack, aStep);
}

G4VParticleChange*
EDepSim::SecondaryEnergy::PostStepDoIt(
    const G4Track& aTrack, const G4Step& aStep)
{
    aParticleChange.Initialize(aTrack);

    if (aStep.GetStepLength() <= 0.0) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    G4double totalEnergyDeposit = aStep.GetTotalEnergyDeposit();
    if (totalEnergyDeposit <= 0.0) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
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

    const G4Material* aMaterial = aTrack.GetMaterial();
    const G4ParticleDefinition *pDef = aParticle->GetDefinition();
    G4String particleName = pDef->GetParticleName();
    EDepSimTrace("Nonionizing " << nonIonizingEnergy
                 << " Total " << totalEnergyDeposit
                 << " Ratio " << nonIonizingEnergy/totalEnergyDeposit
                 << " for " << particleName
                 << " in " << aMaterial->GetName());

    return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

G4double EDepSim::SecondaryEnergy::GetMeanFreePath(
    const G4Track& dummy1, G4double dummy2, G4ForceCondition* condition)
{
    *condition = StronglyForced;
    // Force the EDepSim::SecondaryEnergy physics process to always happen,
    // even if it's not the dominant process.  In effect it is a meta-process
    // on top of any and all other energy depositions which may occur
    // (disregard DBL_MAX, this is "StronglyForced" so it makes the mean free
    // path zero really, not infinite)

    return DBL_MAX;
}

G4double EDepSim::SecondaryEnergy::GetMeanLifeTime(
    const G4Track& dummy1, G4ForceCondition* condition)
{
    *condition = Forced;
    // This function and this condition has the same effect as the above

    return DBL_MAX;
}
