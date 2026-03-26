////////////////////////////////////////////////////////////
//

#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <G4VProcess.hh>
#include <G4TouchableHistory.hh>
#include <G4SDManager.hh>
#include <G4UnitsTable.hh>
#include <G4StepStatus.hh>
#include <G4RunManager.hh>
#include <G4OpticalParameters.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include "EDepSimLog.hh"
#include "EDepSimSurfaceSD.hh"
#include "EDepSimHitSurface.hh"
#include "EDepSimUserStackingAction.hh"

EDepSim::SurfaceSD::SurfaceSD(G4String name)
    :G4VSensitiveDetector(name), fHits(NULL), fHCID(-1) {
    // In an surprising interface, the G4VSensitiveDetector class exposes the
    // protected field "std::vector<G4String> collectionName" to the user and
    // expects any derived classes to explicitly fill it with the names of the
    // hit collections being filled.  The values in collectionName should be
    // accessed through "G4VSensitiveDetector::GetCollectionName(i)".
    collectionName.insert("SurfaceHits");

    // A surface sensitive detector has been created, so make sure that
    // photons will be tracked.
    G4RunManager* theRunManager = G4RunManager::GetRunManager();

    /// Don't check if this is null since it MUST be non-null, and there might
    /// as well be a core dump if it is null.
    EDepSim::UserStackingAction* theStackingAction
        = const_cast<EDepSim::UserStackingAction*>(
            dynamic_cast<const EDepSim::UserStackingAction*>(
                theRunManager->GetUserStackingAction()));

    theStackingAction->SetKillOpticalPhotons(false);

    G4OpticalParameters* opParams = G4OpticalParameters::Instance();
    if (not opParams->GetBoundaryInvokeSD()) {
        EDepSimError("Surface SD will not be involed unless"
                     << " G4OpticalParameters::SetBoundaryInvokeSD"
                     << " is set to true in input macro");
    }
}

EDepSim::SurfaceSD::~SurfaceSD() {}

void EDepSim::SurfaceSD::Initialize(G4HCofThisEvent* HCE) {
    fHits = new EDepSim::HitSurface::HitSurfaceCollection(GetName(),
                                                          GetCollectionName(0));

    if (fHCID<0) {
        G4String hcName = GetName() + "/" + GetCollectionName(0);
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(hcName);
        EDepSimLog("Initialize SD for "
                   << GetName() << "/" << GetCollectionName(0));
    }

    HCE->AddHitsCollection(fHCID, fHits);

}

G4bool EDepSim::SurfaceSD::ProcessHits(G4Step* theStep,
                                       G4TouchableHistory*) {
    // Get the hit information.
    G4double energyDeposit = theStep->GetTotalEnergyDeposit();

    // Check we got some energy deposit.  Having energy isn't enough to say
    // there should be a hit, but there needs to be energy if there is a hit.
    if (energyDeposit <= 0.) return true;

    const G4Track* theTrack = theStep->GetTrack();
    const G4VProcess* theProcess = theTrack->GetCreatorProcess();
    const G4ParticleDefinition* theParticle = theTrack->GetParticleDefinition();
    const G4StepPoint* thePreStep = theStep->GetPreStepPoint();
    const G4StepPoint* thePostStep = theStep->GetPostStepPoint();
    const G4VPhysicalVolume* thePostPV = thePostStep->GetPhysicalVolume();
    const G4ThreeVector& hitPosition = thePostStep->GetPosition();
    const G4double hitTime = thePostStep->GetGlobalTime();
    G4ThreeVector hitPolarization = thePostStep->GetPolarization();
    G4double hitEnergy = thePostStep->GetTotalEnergy();

    std::string processName{"non-set"};
    if (theProcess != nullptr) {
        processName = theProcess->GetProcessName();
    }
    else {
        EDepSimDebug("Missing process for Track"
                     << " " << theTrack->GetTrackID()
                     << " (" << (theParticle->GetParticleName()) << ")"
                     << " Pre-status " << ((thePreStep != nullptr) ?
                                           thePreStep->GetStepStatus()
                                           : -1)
                     << " Post-status " << ((thePostStep != nullptr) ?
                                           thePostStep->GetStepStatus()
                                           : -1));
    }

    EDepSimDebug("Track " << theTrack->GetTrackID()
                 << " (" << (theParticle->GetParticleName())
                 << ")"
                 << " deposit " << energyDeposit
                 << " (" << theStep->GetNonIonizingEnergyDeposit() << ") MeV"
                 << " creation " << processName);

    if (thePostPV != nullptr) {
        EDepSimTrace("   Volume: " << thePostPV->GetName());
    }
    else {
        EDepSimTrace("   Volume: NONE");
    }

    if (theProcess == nullptr) {
        // No process here, so don't create this step.
        return true;
    }

    EDepSimDebug("Create Hit:"
                 << " [" << hitPosition.x()/mm
                 << ", " << hitPosition.y()/mm
                 << ", " << hitPosition.z()/mm
                 << ", " << hitTime/ns << "] mm"
                 << ", " << hitEnergy/eV << " eV"
                 << ", " << twopi*hbarc/hitEnergy/nm << " nm");

    EDepSim::HitSurface* currentHit = new EDepSim::HitSurface(theStep);
    fHits->insert(currentHit);

    return true;
}

void EDepSim::SurfaceSD::EndOfEvent(G4HCofThisEvent*) { }
