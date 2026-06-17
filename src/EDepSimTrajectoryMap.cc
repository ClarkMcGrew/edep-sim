#include "EDepSimTrajectoryMap.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimException.hh"
#include "EDepSimUserEventInformation.hh"

#include <G4VTrajectory.hh>
#include <G4VTrajectoryPoint.hh>
#include <G4ThreeVector.hh>
#include <G4EventManager.hh>

#include "EDepSimLog.hh"
#include "EDepSimBacktrace.hh"

void EDepSim::TrajectoryMap::Add(G4VTrajectory* traj, G4Event* event) {
    if (!event) {
        event = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
    }
    if (!event) {
        EDepSimError("Backtrace " << std::endl
                     << EDepSim::Backtrace);
        EDepSimError("Invalid Event Pointer");
        throw std::runtime_error("Bad event pointer");
    }
    EDepSim::UserEventInformation* info
        = dynamic_cast<EDepSim::UserEventInformation*>(
            event->GetUserInformation());
    if (!info) {
        EDepSimError("Backtrace " << std::endl
                     << EDepSim::Backtrace);
        EDepSimError("Invalid Event Information Pointer");
        throw std::runtime_error("Bad event information pointer");
    }
    EDepSim::Trajectory * update = dynamic_cast<EDepSim::Trajectory*>(traj);
    if (update == nullptr) {
        EDepSimError("Trajectory must be an EDepSim::Trajectory");
        EDepSimError("Backtrace " << std::endl
                 << EDepSim::Backtrace);
        std::abort();
    }
    int trackId = update->GetTrackID();
    G4VTrajectory* oldTraj = EDepSim::TrajectoryMap::Get(trackId,event);
    if (oldTraj == nullptr) {
        // The trajectory doesn't exist, so add it
        info->fMap[trackId] = update;
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

int EDepSim::TrajectoryMap::FindPrimaryId(int trackId,
                                          const G4Event* event) {
    if (!event) {
        event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    }
    if (!event) {
        EDepSimError("Backtrace " << std::endl
                     << EDepSim::Backtrace);
        EDepSimError("Invalid Event Pointer");
        throw std::runtime_error("Bad event pointer");
    }
    int currentId = trackId;
    int parentId = trackId;
    int loopCount=0;
    for (loopCount=0;loopCount<10000;++loopCount) {
        G4VTrajectory* t = Get(currentId,event);
        if (!t) break;
        parentId = t->GetParentID();
        // Check to see the search loop should terminate.
        G4VTrajectory* p = Get(parentId,event);
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

G4VTrajectory* EDepSim::TrajectoryMap::Get(int trackId,
                                           const G4Event* event) {
    if (!event) {
        event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    }
    if (!event) {
        EDepSimError("Backtrace " << std::endl
                     << EDepSim::Backtrace);
        EDepSimError("Invalid Event Pointer");
        throw std::runtime_error("Bad event pointer");
    }
    EDepSim::UserEventInformation* info
        = dynamic_cast<EDepSim::UserEventInformation*>(
            event->GetUserInformation());
    if (!info) {
        EDepSimError("Backtrace " << std::endl
                     << EDepSim::Backtrace);
        EDepSimError("Invalid Event Information Pointer");
        throw std::runtime_error("Bad event information pointer");
    }
    std::map<int,G4VTrajectory*>::iterator t = info->fMap.find(trackId);
    if (t == info->fMap.end()) {
        return NULL;
    }
    return t->second;
}
