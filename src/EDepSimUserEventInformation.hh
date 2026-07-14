////////////////////////////////////////////////////////
// $Id: EDepSim::UserEventInformation.hh,v 1.5 2008/01/14 17:01:57 mcgrew Exp $

#ifndef EDepSim_UserEventInformation_hh_seen
#define EDepSim_UserEventInformation_hh_seen

#include <G4VUserEventInformation.hh>
#include <G4PrimaryVertex.hh>
#include <G4VTrajectory.hh>

#include <map>

namespace EDepSim {
    class UserEventInformation;
    class TrajectoryMap;
}
class EDepSim::UserEventInformation : public G4VUserEventInformation {
    friend EDepSim::TrajectoryMap;

public:
    UserEventInformation();
    virtual ~UserEventInformation();

    /// Print information about the event.
    virtual void Print() const;

    /// Reset for a new event.  This is needed since some event generators
    /// start creating an event, and then later decide to reject it.  This
    /// allows the user information to be reset to the default values.
    void InitializeEvent(void);

private:

    /// A map to the trajectories information indexed the the track id. Be
    /// careful since the trajectory information is owned by the G4Event, so
    /// if you try to use this after a trajectory has been deleted... bad
    /// things will happen.  That should never happen since the
    /// UserEventInformation is also owned by the event.  This is directly
    /// access by TrajectoryMap (which is a friend).
    std::map<int, G4VTrajectory*> fMap;
};
#endif
