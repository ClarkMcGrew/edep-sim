#include "EDepSimDokeBirksSaturation.hh"
#include "EDepSimLog.hh"

#include <G4EmSaturation.hh>
#include <G4SystemOfUnits.hh>
#include <G4ParticleDefinition.hh>
#include <G4Material.hh>
#include <G4MaterialCutsCouple.hh>
#include <G4EventManager.hh>
#include <G4TrackingManager.hh>
#include <G4Track.hh>
#include <G4FieldManager.hh>
#include <G4Field.hh>

EDepSim::DokeBirksSaturation::DokeBirksSaturation(G4int verb)
    : G4EmSaturation(verb) {}

EDepSim::DokeBirksSaturation::~DokeBirksSaturation() {}

// The physics in EDepSim::DokeBirksSaturation is shamelessly stolen from
// NEST, so cite the NEST paper to give credit where credit is due.  However,
// all of the bugs are mine, so there should be a note like "Simplified
// implementation of the physics described in NEST paper".
G4double EDepSim::DokeBirksSaturation::VisibleEnergyDeposition(
    const G4ParticleDefinition *particle,
    const G4MaterialCutsCouple *couple,
    G4double length,
    G4double totalEDep,
    G4double nonIonEDep) const {
    EDepSimTrace("Using DokeBirksSaturation "
                 << totalEDep << " "
                 << length);

    const G4Material* aMaterial = couple->GetMaterial();

    // Assume that this is argon.
    bool inLiquidArgon = true;

    // Check that we are in liquid.
    if (aMaterial->GetState() != kStateLiquid) {
        if (aMaterial->GetState() == kStateUndefined) {
            EDepSimError("Undefined material state for "
                         << aMaterial->GetName());
        }
        inLiquidArgon = false;
    }

    // Find the dominant element.  It should be argon.
    double dominantZ = -1;
    double dominantFrac = 0.0;
    for (std::size_t ele = 0; ele < aMaterial->GetNumberOfElements(); ++ele) {
        double frac = aMaterial->GetFractionVector()[ele];
        if (frac > dominantFrac) {
            dominantFrac = frac;
            dominantZ = aMaterial->GetElement(ele)->GetZ();
        }
    }

    // If the dominant fraction is small its not LAr.  Hard coded to allow 10
    // PPM contamination.  More than that, there won't be drift anyway, and
    // it's OK to fall back to something simpler.
    if (dominantFrac < (1.0 - 1E-5)) {
        inLiquidArgon = false;
    }

    // Check that the element is argon.
    if (std::abs(dominantZ-18.0) > 0.5) {
        inLiquidArgon = false;
    }

    // It's not LAr so pass off to the standard handler.
    if (!inLiquidArgon) {
        return G4EmSaturation::VisibleEnergyDeposition(
            particle,couple,length,totalEDep,nonIonEDep);
    }

    EDepSimTrace("In LAr");

    G4String particleName = particle->GetParticleName();
    // G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
    G4double totalEnergyDeposit = totalEDep - nonIonEDep;

    if (totalEnergyDeposit <= 0.0) {
        return 0.0;
    }

    if (length <= 0.0) {
        return 0.0;
    }

    if (nonIonEDep > 0.0) {
        EDepSimWarn("Something else is using non-ionizing energy");
    }

    // To go forward we need a little more information: The electric field,
    // and the step number to check if this is a "stopped" electron.
    G4Track* aTrack = G4EventManager::GetEventManager()
        ->GetTrackingManager()->GetTrack();

    const G4Step* aStep = aTrack->GetStep();

    // Figure out the electric field for the volume.
    double electricField = 0.0;
    do {
        G4StepPoint* pPostStepPoint = aStep->GetPostStepPoint();
        const G4VPhysicalVolume* aVolume = pPostStepPoint->GetPhysicalVolume();
        const G4LogicalVolume* aLogVolume = aVolume->GetLogicalVolume();
        const G4FieldManager* aFieldManager = aLogVolume->GetFieldManager();
        if (!aFieldManager) {
            EDepSimDebug("No Field Manager");
            break;
        }
        if (!aFieldManager->DoesFieldExist()) {
            EDepSimDebug("Field doesn't exist");
            break;
        }
        const G4Field* aField = aFieldManager->GetDetectorField();
        if (!aField) {
            EDepSimDebug("LAr Volume "
                         << aLogVolume->GetName()
                         << " should have field!");
            break;
        }

        G4ThreeVector thePosition = pPostStepPoint->GetPosition();
        double theTime = pPostStepPoint->GetGlobalTime();
        double point[4] = {
            thePosition.x(), thePosition.y(), thePosition.z(), theTime};
        double bField[6];
        aField->GetFieldValue(point,bField);

        electricField = bField[3]*bField[3];
        electricField += bField[4]*bField[4];
        electricField += bField[5]*bField[5];
        electricField = std::sqrt(electricField);

        if (not std::isfinite(electricField)) {
            EDepSimError("Electric field must be valid");
            throw std::runtime_error("Electric field must be valid.");
        }
    } while (false);

    // The code below is pulled from G4S1Light and is simplified to be
    // Doke-Birks only, in LAr only, and for an electric field only.  This is
    // for ARGON only.  The Doke-Birks constants are in kilovolt/cm
    G4double dokeBirks[3];

    dokeBirks[0] = 0.07*pow((electricField),-0.85);
    dokeBirks[2] = 0.00;
    dokeBirks[1] = dokeBirks[0]/(1-dokeBirks[2]); //B=A/(1-C) (see paper)

    G4double dE = totalEDep/MeV;
    G4double dx = length/cm;
    G4double density = aMaterial->GetDensity()/(g/cm3);
    G4double LET = 0.0; //lin. energy transfer (prop. to dE/dx)
    if (dx != 0.0) LET = (dE/dx)*(1/density);

    // There was some dEdX for a photon.  This means that the very low energy
    // electrons were not simulated.
    if (particleName == "gamma") {
        LET = CalculateElectronLET( 1000*dE );
        dx = dE/density/LET;
    }
    else if (std::abs(particle->GetPDGCharge()) < 0.1) {
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

    // Special case for electrons that G4 may stop when they get very short
    // and truncate the energy deposition.
    do {
        if (particleName != "e+" || particleName != "e-") break;
        if (aTrack->GetCurrentStepNumber() != 1) break;
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

    EDepSimTrace("DokeBirks: " << nonIonizingEnergy
                 << " Total " << totalEnergyDeposit
                 << " Ratio " << nonIonizingEnergy/totalEnergyDeposit);

    return nonIonizingEnergy;
}

// Get the expected electron electron dEdX/density as a function of energy.
// The energy must be in MeV for the parametrization to work.  This is only
// applicable to argon.
//
// PHYSICS CHECK: Every place this is used, the energy is converted to KeV
// before the call.  A quick look at NEST suggests the energy should be in
// KeV, but either my code or my comment is wrong.
G4double EDepSim::DokeBirksSaturation::CalculateElectronLET ( G4double E) const {
    G4double LET;
    if ( E >= 1 ) LET = 116.70-162.97*log10(E)+99.361*pow(log10(E),2)-
                      33.405*pow(log10(E),3)+6.5069*pow(log10(E),4)-
                      0.69334*pow(log10(E),5)+.031563*pow(log10(E),6);
    else if ( E>0 && E<1 ) LET = 100;
    else LET = 0;
    return LET;
}
