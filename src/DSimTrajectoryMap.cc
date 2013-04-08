#include "DSimTrajectoryMap.hh"
#include "DSimTrajectory.hh"
#include "DSimException.hh"

#include <G4VTrajectory.hh>
#include <G4VTrajectoryPoint.hh>
#include <G4ThreeVector.hh>

#include <TCaptLog.hxx>

std::map<int, G4VTrajectory*> DSimTrajectoryMap::fMap;

void DSimTrajectoryMap::Clear() {
    fMap.clear();
}

void DSimTrajectoryMap::Add(G4VTrajectory* traj) {
        int trackId = traj->GetTrackID();
        fMap[trackId] = traj;
}

int DSimTrajectoryMap::FindPrimaryId(int trackId) {
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
        DSimTrajectory * dsimTraj = dynamic_cast<DSimTrajectory*>(t);
        if (!dsimTraj) DSimError("Invalid Trajectory");
        if (dsimTraj->GetProcessName() == "Decay") break;
        // A parent ID of zero means that this particle is a primary particle,
        // so that makes it a primary trajectory too.
        if (parentId == 0) break;
        currentId = parentId;
    }
    if (loopCount>9999) {
        CaptLog("Infinite Loop in DSimTrajectoryMap::FindPrimaryId(): "
                 << "Track Id: " << trackId);
    }
    
    return currentId;
}

G4VTrajectory* DSimTrajectoryMap::Get(int trackId) {
    std::map<int,G4VTrajectory*>::iterator t = fMap.find(trackId);
    if (t == fMap.end()) {
        return NULL;
    }
    return t->second;
}

