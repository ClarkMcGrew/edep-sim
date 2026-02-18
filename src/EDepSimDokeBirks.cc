#include <G4ParticleTypes.hh> //lets you refer to G4OpticalPhoton, etc.
#include <G4EmProcessSubType.hh> //lets you call this process Scintillation
#include <G4Version.hh> //tells you what Geant4 version you are running
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>
#include <G4FieldManager.hh>
#include <G4Field.hh>


#include "EDepSimLog.hh"

#include "EDepSimDokeBirks.hh"

// The physics in EDepSim::DokeBirks is shamelessly stolen from NEST, so cite
// the NEST paper to give credit where credit is due.  However, all of the
// bugs are mine, so there should be a note like "Simplified implementation of
// the physics described in NEST paper".
EDepSim::DokeBirks::DokeBirks(const G4String& processName,
                             G4ProcessType type)
    : G4VRestDiscreteProcess(processName, type),
      fEmSaturation(nullptr)
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

    bool inLiquidArgon = true;
    // Check that we are in liquid argon.
    if (aMaterial->GetState() != kStateLiquid) {
        // Do Nothing
        if (aMaterial->GetState() == kStateUndefined) {
            EDepSimError("Undefined material state for "
                         << aMaterial->GetName());
        }
        inLiquidArgon = false;
    }
    if (aMaterial->GetNumberOfElements() > 1) {
        // Do Nothing
        inLiquidArgon = false;
    }
    if (std::abs(aMaterial->GetElement(0)->GetZ()-18.0) > 0.5) {
        // Do Nothing
        inLiquidArgon = false;
    }

    const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
    const G4ParticleDefinition *pDef = aParticle->GetDefinition();
    G4String particleName = pDef->GetParticleName();
    // G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
    G4double totalEnergyDeposit = aStep.GetTotalEnergyDeposit();

    if (totalEnergyDeposit <= 0.0) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    if (aStep.GetStepLength() <= 0.0) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    if (!inLiquidArgon) {
#if G4VERSION_NUMBER >= 1100
        // The G4 EMSaturation model is update to expect a charged particle.
        if (std::abs(aParticle->GetCharge()) < 0.1) {
            return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }
#endif
        if (!fEmSaturation) fEmSaturation = new G4EmSaturation(0);
        G4double nonIonizingEnergy
            = fEmSaturation->VisibleEnergyDepositionAtAStep(&aStep);
        aParticleChange.ProposeNonIonizingEnergyDeposit(nonIonizingEnergy);
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    // Figure out the electric field for the volume.
    G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
    const G4VPhysicalVolume* aVolume = pPostStepPoint->GetPhysicalVolume();
    const G4LogicalVolume* aLogVolume = aVolume->GetLogicalVolume();
    const G4FieldManager* aFieldManager = aLogVolume->GetFieldManager();
    if (!aFieldManager) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
    if (!aFieldManager->DoesFieldExist()) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
    const G4Field* aField = aFieldManager->GetDetectorField();
    if (!aField) {
        EDepSimError("LAr Volume "
                     << aLogVolume->GetName()
                     << " should have field!");
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    G4ThreeVector thePosition = pPostStepPoint->GetPosition();
    double theTime = pPostStepPoint->GetGlobalTime();
    double point[4] = {
        thePosition.x(), thePosition.y(), thePosition.z(), theTime};
    double bField[6];
    aField->GetFieldValue(point,bField);

    double electricField = bField[3]*bField[3];
    electricField += bField[4]*bField[4];
    electricField += bField[5]*bField[5];
    if (electricField > 0) {
        electricField = std::sqrt(electricField);
    }
    // The code below is pulled from G4S1Light and is simplified to be
    // Doke-Birks only, in LAr only, and for an electric field only.  This is
    // for ARGON only.  The Doke-Birks constants are in kilovolt/cm
    G4double dokeBirks[3];

    dokeBirks[0] = 0.07*pow((electricField),-0.85);
    dokeBirks[2] = 0.00;
    dokeBirks[1] = dokeBirks[0]/(1-dokeBirks[2]); //B=A/(1-C) (see paper)

    G4double dE = totalEnergyDeposit/MeV;
    G4double dx = aStep.GetStepLength()/cm;
    G4double density = aMaterial->GetDensity()/(g/cm3);
    G4double LET = 0.0; //lin. energy transfer (prop. to dE/dx)
    if (dx != 0.0) LET = (dE/dx)*(1/density);

    // There was some dEdX for a photon.  This means that the very low energy
    // electrons were not simulated.
    if (particleName == "gamma") {
        LET = CalculateElectronLET( 1000*dE );
        dx = dE/density/LET;
    }
    else if (std::abs(pDef->GetPDGCharge()) < 0.1) {
        // Handle the case where a neutral particle made an "electron".  This
        // is when there is a tiny electron scatter that is below the G4
        // threshold.  The assumption is the same as NEST.  The product
        // particle is an electron.
        double tempLET = CalculateElectronLET( 1000*dE );
        double electronStep = dE/density/tempLET;
        if (dx > electronStep) {
            LET = tempLET;
            dx = electronStep;
        }
    }

    // Special case for electrons that G4 may stop when they get to short and
    // truncate the energy deposition.
    do {
        if (particleName != "e+" || particleName != "e-") break;
        if (aTrack.GetCurrentStepNumber() != 1) break;
        double ratio = CalculateElectronLET( 1000.0*dE ) / LET;
        if (ratio > 0.7) break;
        LET *= ratio;
        dx /= ratio;
    } while (false);

    G4double recombProb = (dokeBirks[0]*LET)/(1+dokeBirks[1]*LET)+dokeBirks[2];

    G4double ScintillationYield = 1 / (19.5*eV);
    G4double NumQuanta = ScintillationYield*totalEnergyDeposit;
    G4double ExcitationRatio = 0.21; //Aprile et. al book
    G4double NumExcitons = NumQuanta*ExcitationRatio/(1+ExcitationRatio);
    G4double NumIons = NumQuanta - NumExcitons;
    G4double NumPhotons = NumExcitons + NumIons*recombProb;

    // The number of photons and electrons for this step has been calculated
    // using the Doke/Birks law version of recombination.  For DETSIM we are
    // only looking at argon, and according the Matt both Doke-Birks and
    // Thomas-Imel fit the data.  It's also because we are not interested in
    // extremely low energies, so Thomas-Imel makes a negligible correction.
    G4double nonIonizingEnergy = totalEnergyDeposit;
    nonIonizingEnergy *= NumPhotons/NumQuanta;
    aParticleChange.ProposeNonIonizingEnergyDeposit(nonIonizingEnergy);
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
    // of any and all other energy depositions which may occur, just like the
    // original G4Scintillation (disregard DBL_MAX, this function makes the
    // mean free path zero really, not infinite)

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

// Get the expected electron electron dEdX as a function of energy.  This is
// normalized to density.  The energy must be in MeV for the parametrization
// to work.  This is only applicable to argon.
G4double EDepSim::DokeBirks::CalculateElectronLET ( G4double E) {
    G4double LET;
    if ( E >= 1 ) LET = 116.70-162.97*log10(E)+99.361*pow(log10(E),2)-
                      33.405*pow(log10(E),3)+6.5069*pow(log10(E),4)-
                      0.69334*pow(log10(E),5)+.031563*pow(log10(E),6);
    else if ( E>0 && E<1 ) LET = 100;
    else LET = 0;
    return LET;
}
