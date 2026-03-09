////////////////////////////////////////////////////////////
// $Id: EDepSim::UserTrackingAction.cc,v 1.6 2007/01/01 05:32:49 mcgrew Exp $
//
#include "EDepSimUserTrackingAction.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimLog.hh"
#include "EDepSimBacktrace.hh"

#include "globals.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"

EDepSim::UserTrackingAction::UserTrackingAction() {}

EDepSim::UserTrackingAction::~UserTrackingAction() {}

void EDepSim::UserTrackingAction::PreUserTrackingAction(const G4Track* trk) {
    EDepSimTrace("Pre-tracking action");
    int trackId = trk->GetTrackID();

    // Make a new trajectory and give it to the tracking manager.  This might
    // be a continuation of an existing track, but that is OK since the
    // trajectories will be merged later on.
    G4VTrajectory* traj = new EDepSim::Trajectory(trk);
    fpTrackingManager->SetTrajectory(traj);
    fpTrackingManager->SetStoreTrajectory(true);

    // Check if the trajectory is in the map already. Only add the first
    // one. If the trackID is already in the map, then this is a continuation
    // of an existing track.
    G4VTrajectory* oldTraj = EDepSim::TrajectoryMap::Get(trackId);
    if (oldTraj != nullptr) {
        EDepSim::Trajectory * update
            = dynamic_cast<EDepSim::Trajectory*>(oldTraj);
        if (update == nullptr) {
            EDepSimError("Not an EDepSim Trajectory " << std::endl
                         << EDepSim::Backtrace);
            throw;
        }
        return;
    }

    EDepSim::TrajectoryMap::Add(traj);
}

void EDepSim::UserTrackingAction::PostUserTrackingAction(const G4Track*) {
    EDepSimTrace("Post-tracking action");
}
