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
    :  fPrimaryId(0), fEnergyDeposit(0),
       fPosition(0,0,0,0), fStart(0,0,0,0) {
}

EDepSim::HitSurface::HitSurface(const EDepSim::HitSurface& rhs)
    : G4VHit(rhs),
      fContributors(rhs.fContributors), fPrimaryId(rhs.fPrimaryId),
      fEnergyDeposit(rhs.fEnergyDeposit),
      fPosition(rhs.fPosition), fStart(rhs.fStart) {}


EDepSim::HitSurface::~HitSurface() { }

int EDepSim::HitSurface::FindPrimaryId(G4Track *theTrack) {
    return EDepSim::TrajectoryMap::FindPrimaryId(theTrack->GetTrackID());
}

void EDepSim::HitSurface::Draw(void) {
}

void EDepSim::HitSurface::Print(void) {
    std::cout << "Hit Energy Deposit: "
              << G4BestUnit(fEnergyDeposit,"Energy")
              << std::endl;
}
