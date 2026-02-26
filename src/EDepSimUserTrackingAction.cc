////////////////////////////////////////////////////////////
// $Id: EDepSim::UserTrackingAction.cc,v 1.6 2007/01/01 05:32:49 mcgrew Exp $
//
#include "EDepSimUserTrackingAction.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimLog.hh"

#include "globals.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

EDepSim::UserTrackingAction::UserTrackingAction() {}

EDepSim::UserTrackingAction::~UserTrackingAction() {}

void EDepSim::UserTrackingAction::PreUserTrackingAction(const G4Track* trk) {
    EDepSimTrace("Pre-tracking action ");

    G4VTrajectory* traj = new EDepSim::Trajectory(trk);
    fpTrackingManager->SetTrajectory(traj);
    fpTrackingManager->SetStoreTrajectory(true);
    EDepSim::TrajectoryMap::Add(traj);
}

void EDepSim::UserTrackingAction::PostUserTrackingAction(const G4Track*) {
    EDepSimTrace("Post-tracking action");
}
