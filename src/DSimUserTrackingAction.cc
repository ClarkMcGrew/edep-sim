////////////////////////////////////////////////////////////
// $Id: DSimUserTrackingAction.cc,v 1.6 2007/01/01 05:32:49 mcgrew Exp $
//

#include "globals.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

#include "DSimUserTrackingAction.hh"
#include "DSimTrajectory.hh"
#include "DSimTrajectoryMap.hh"

DSimUserTrackingAction::DSimUserTrackingAction() {}

DSimUserTrackingAction::~DSimUserTrackingAction() {}

void DSimUserTrackingAction::PreUserTrackingAction(const G4Track* trk) {
    G4VTrajectory* traj = new DSimTrajectory(trk);
    fpTrackingManager->SetTrajectory(traj);
    fpTrackingManager->SetStoreTrajectory(true);
    DSimTrajectoryMap::Add(traj);
}

