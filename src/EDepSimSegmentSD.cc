////////////////////////////////////////////////////////////
// $Id: EDepSim::SegmentSD.cc,v 1.6 2008/09/03 17:33:42 mcgrew Exp $
//

#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <G4VProcess.hh>
#include <G4TouchableHistory.hh>
#include <G4SDManager.hh>
#include <G4UnitsTable.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <EDepSimLog.hh>

#include "EDepSimSegmentSD.hh"
#include "EDepSimHitSegment.hh"

EDepSim::SegmentSD::SegmentSD(G4String name)
    :G4VSensitiveDetector(name),
     fHits(NULL), fHCID(-1),
     fMaximumHitSagitta(1*CLHEP::mm), fMaximumHitLength(3*CLHEP::mm),
     fLastHit(0) {
    // In an unbelievably poor interface, the G4VSensitiveDetector class
    // exposes the protected field "std::vector<G4String> collectionName" to
    // the user and expects any derived classes to explicitly fill it with the
    // names of the hit collection filled.  The values in collectionName
    // should be accessed through
    // "G4VSensitiveDetector::GetCollectionName(i)".
    collectionName.insert("SegmentHits");
}

EDepSim::SegmentSD::~SegmentSD() {}

void EDepSim::SegmentSD::Initialize(G4HCofThisEvent* HCE) {
    fHits = new EDepSim::HitSegment::HitSegmentCollection(GetName(),
                                                     GetCollectionName(0));

    if (fHCID<0) {
        G4String hcName = GetName() + "/" + GetCollectionName(0);
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(hcName);
        EDepSimLog("Initialize SD for "
                  << GetName() << "/" << GetCollectionName(0)
                  << " w/ sagitta " << G4BestUnit(fMaximumHitSagitta,"Length")
                  << " & " << G4BestUnit(fMaximumHitLength,"Length"));
    }
    HCE->AddHitsCollection(fHCID, fHits);
}

G4bool EDepSim::SegmentSD::ProcessHits(G4Step* theStep,
                                       G4TouchableHistory*) {
    // Get the hit information.
    G4double energyDeposit = theStep->GetTotalEnergyDeposit();
    if (energyDeposit <= 0.) return true;

    EDepSimTrace("Process hit with " << energyDeposit
                 << " in " << theStep->GetTrack()->GetVolume()->GetName());


    EDepSim::HitSegment* currentHit = NULL;

    // Check to see if the last hit in the vector of hits needs to be
    // expanded.
    if (0<=fLastHit && fLastHit < (int) fHits->entries()) {
        EDepSim::HitSegment *tmpHit = (*fHits)[fLastHit];
        if (tmpHit->SameHit(theStep)) {
            currentHit = tmpHit;
        }
    }

    // If a hit wasn't found, create one.
    if (!currentHit) {
        currentHit = new EDepSim::HitSegment(fMaximumHitSagitta,
                                             fMaximumHitLength);
        fLastHit = fHits->entries();
        fHits->insert(currentHit);
    }

    currentHit->AddStep(theStep);

    return true;
}

void EDepSim::SegmentSD::EndOfEvent(G4HCofThisEvent*) { }
