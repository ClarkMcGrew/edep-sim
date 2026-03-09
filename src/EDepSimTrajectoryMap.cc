#include "EDepSimTrajectoryMap.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimException.hh"

#include <G4VTrajectory.hh>
#include <G4VTrajectoryPoint.hh>
#include <G4ThreeVector.hh>

#include "EDepSimLog.hh"
#include "EDepSimBacktrace.hh"

std::map<int, G4VTrajectory*> EDepSim::TrajectoryMap::fMap;

void EDepSim::TrajectoryMap::Clear() {
    fMap.clear();
}

void EDepSim::TrajectoryMap::Add(G4VTrajectory* traj) {
    EDepSim::Trajectory * update = dynamic_cast<EDepSim::Trajectory*>(traj);
    if (update == nullptr) {
        EDepSimError("Trajectory must be an EDepSim::Trajectory");
        EDepSimError("Backtrace " << std::endl
                 << EDepSim::Backtrace);
        std::abort();
    }
    int trackId = update->GetTrackID();
    G4VTrajectory* oldTraj = EDepSim::TrajectoryMap::Get(trackId);
    if (oldTraj == nullptr) {
        // The trajectory doesn't exist, so add it
        fMap[trackId] = update;
        return;
    }
    // Check that the new trajectory matches the old trajectory.  Shouldn't
    // happen, but allow it with a error message.
    if (oldTraj == update) {
        EDepSimError("Existing trajectory added to TrajectoryMap");
        return;
    }
    // Fail!  This must not happen.
    EDepSimError("Cannot overwrite an existing trajectory");
    EDepSimError("Backtrace " << std::endl
                 << EDepSim::Backtrace);
    std::abort();
}

int EDepSim::TrajectoryMap::FindPrimaryId(int trackId) {
    int currentId = trackId;
    int parentId = trackId;
    int loopCount=0;
    for (loopCount=0;loopCount<10000;++loopCount) {
        G4VTrajectory* t = Get(currentId);
        if (!t) break;
        parentId = t->GetParentID();
        // Check to see the search loop should terminate.
        G4VTrajectory* p = Get(parentId);
        // There is no parent so break so this is a primary trajectory.
        if (!p) break;
        // Decay products are primary trajectories since they should be
        // independently reconstructed
        EDepSim::Trajectory * edepTraj = dynamic_cast<EDepSim::Trajectory*>(t);
        if (!edepTraj) EDepSimThrow("Invalid Trajectory");
        if (edepTraj->GetProcessName() == "Decay") break;
        // A parent ID of zero means that this particle is a primary particle,
        // so that makes it a primary trajectory too.
        if (parentId == 0) break;
        currentId = parentId;
    }
    if (loopCount>9999) {
        EDepSimLog("Infinite Loop in EDepSim::TrajectoryMap::FindPrimaryId(): "
                 << "Track Id: " << trackId);
    }

    return currentId;
}

G4VTrajectory* EDepSim::TrajectoryMap::Get(int trackId) {
    std::map<int,G4VTrajectory*>::iterator t = fMap.find(trackId);
    if (t == fMap.end()) {
        return NULL;
    }
    return t->second;
}
