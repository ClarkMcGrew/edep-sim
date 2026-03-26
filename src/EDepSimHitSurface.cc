////////////////////////////////////////////////////////////
// Define a to hold information about particles that hit a sensitive surface
// (e.g. a photo cathode)

#include "EDepSimRootGeometryManager.hh"
#include "EDepSimHitSurface.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimLog.hh"

#include <G4EventManager.hh>
#include <G4Event.hh>
#include <G4VTrajectory.hh>
#include <G4VTrajectoryPoint.hh>
#include <G4TrajectoryContainer.hh>
#include <G4TouchableHandle.hh>
#include <G4Step.hh>
#include <G4ParticleDefinition.hh>

#include <G4UnitsTable.hh>
#include <G4VisAttributes.hh>
#include <G4VVisManager.hh>
#include <G4Polyline.hh>
#include <G4Color.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <cmath>
#include <iostream>
#include <map>
#include <sstream>

G4Allocator<EDepSim::HitSurface> edepHitSurfaceAllocator;

EDepSim::HitSurface::HitSurface()
    :  fPrimaryId(-1), fEnergyDeposit(0),
       fPosition(0,0,0,0), fStart(0,0,0,0),
       fPDGEncoding(0), fCreatorType(-1), fCreatorSubtype(-1) {}

EDepSim::HitSurface::HitSurface(const G4Step* theStep)
    :  fPrimaryId(0), fEnergyDeposit(0),
       fPosition(0,0,0,0), fStart(0,0,0,0),
       fPDGEncoding(0), fCreatorType(-1), fCreatorSubtype(-1) {
    fPrimaryId = theStep->GetTrack()->GetParentID();
    fEnergyDeposit = theStep->GetTotalEnergyDeposit();
    fPosition = G4LorentzVector(theStep->GetPostStepPoint()->GetPosition(),
                                theStep->GetPostStepPoint()->GetGlobalTime());
    fStart = G4LorentzVector(theStep->GetTrack()->GetPosition(),
                             theStep->GetTrack()->GetGlobalTime());
    fPDGEncoding
        = theStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    const G4VProcess* theProcess = theStep->GetTrack()->GetCreatorProcess();
    if (theProcess != nullptr) {
        fCreatorType = theProcess->GetProcessType();
        fCreatorSubtype = theProcess->GetProcessSubType();
    }
}

EDepSim::HitSurface::~HitSurface() { }

void EDepSim::HitSurface::Draw(void) {
}

void EDepSim::HitSurface::Print(void) {
    std::cout << "Hit Energy Deposit: "
              << G4BestUnit(fEnergyDeposit,"Energy")
              << std::endl;
}
