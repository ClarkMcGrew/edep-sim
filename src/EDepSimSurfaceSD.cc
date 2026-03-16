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

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <EDepSimLog.hh>

#include "EDepSimSurfaceSD.hh"
#include "EDepSimHitSurface.hh"

EDepSim::SurfaceSD::SurfaceSD(G4String name)
    :G4VSensitiveDetector(name), fHits(NULL), fHCID(-1) {
    // In an surprising interface, the G4VSensitiveDetector class exposes the
    // protected field "std::vector<G4String> collectionName" to the user and
    // expects any derived classes to explicitly fill it with the names of the
    // hit collections being filled.  The values in collectionName should be
    // accessed through "G4VSensitiveDetector::GetCollectionName(i)".
    collectionName.insert("SurfaceHits");
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
    const G4Track* theTrack = theStep->GetTrack();
    const G4ParticleDefinition* theParticle = theTrack->GetParticleDefinition();

    EDepSimDebug("Track " << theTrack->GetTrackID()
                 << " (" << (theParticle->GetParticleName())
                 << ")"
                 << " deposit " << energyDeposit
                 << " (" << theStep->GetNonIonizingEnergyDeposit() << ") MeV");
    EDepSimTrace("   Volume: " << theTrack->GetVolume()->GetName());

    // Check we got some energy deposit.  Having energy isn't enough to say
    // there should be a hit, but there needs to be energy if there is a hit.
    if (energyDeposit <= 0.) return true;

    return true;
}

void EDepSim::SurfaceSD::EndOfEvent(G4HCofThisEvent*) { }
