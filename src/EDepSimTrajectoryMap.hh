#ifndef EDepSim_TrajectoryMap_hh_seen
#define EDepSim_TrajectoryMap_hh_seen
////////////////////////////////////////////////////////////
// $Id: EDepSim::TrajectoryMap.hh,v 1.1 2007/01/01 05:32:49 mcgrew Exp $
//

#include <map>

class G4VTrajectory;

/// Maintain a the track Id to the trajectory in the trajectory container for
/// this event. This could be implemented directly using find and the
/// G4TrajectoryContainer vector, but that seems like it's depending on an
/// internal implementation detail.  Instead, this maintains a std::map
/// between the integer trajectory id and the trajectory pointer.
namespace EDepSim {class TrajectoryMap;}
class EDepSim::TrajectoryMap {
public:
    ~TrajectoryMap() {}

    /// Provide a map between the track id and the trajectory object.
    static G4VTrajectory* Get(int trackId);

    /// Add a trajectory to the map.
    static void Add(G4VTrajectory* traj);

    /// Clear the trajectory map.  This must be done in the
    /// EDepSim::UserEventAction::BeginOfEventAction() method.
    static void Clear();

    /// Find the primary track ID for the current track.  This is the primary
    /// that is the ultimate parent of the current track.
    static int FindPrimaryId(int trackId);

private:
    /// Provide a getter for the map.  This could be setup to return a
    /// lock_guard for RAII.  The returned object needs to act like the map.
    static std::map<int,G4VTrajectory*>& GetMap();

    /// The constructor is private so that it can only be created using the
    /// static get method.
    TrajectoryMap() {}
};
#endif
