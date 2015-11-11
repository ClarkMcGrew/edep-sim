#ifndef DSimTrajectoryPoint_hh_seen
#define DSimTrajectoryPoint_hh_seen

#include <globals.hh>
#include <G4TrajectoryPoint.hh>
#include <G4ThreeVector.hh>
#include <G4Allocator.hh>
#include <G4StepStatus.hh>
#include <G4ProcessType.hh>

class G4Track;
class G4Step;
class G4VProcess;

/// Store a point along a particle trajectory.  This is used internally for
/// some bookkeeping, and then is translated into an output class when the
/// event is saved.
class DSimTrajectoryPoint : public G4TrajectoryPoint {
public:
    DSimTrajectoryPoint();
    DSimTrajectoryPoint(const G4Track* aTrack);
    DSimTrajectoryPoint(const G4Step* aStep);
    DSimTrajectoryPoint(const DSimTrajectoryPoint &right);
    virtual ~DSimTrajectoryPoint();

    inline void *operator new(size_t);
    inline void operator delete(void *aDSimTrajectoryPoint);
    inline int operator==(const DSimTrajectoryPoint& right) const
    { return (this==&right); };

    /// Get the time that the particle passed this trajectory point.
    G4double GetTime() const { return fTime; }

    /// Get the 3-momentum of the particle at this trajectory point.
    const G4ThreeVector GetMomentum() const { return fMomentum; }

    /// Get the G4 stepping status of the interaction that instigated this
    /// trajectory point.
    G4StepStatus GetStepStatus() const { return fStepStatus; }

    /// Get the process type for this point (the process types are defined in
    /// G4ProcessType.hh).
    G4ProcessType GetProcessType() const { return fProcessType; }

    /// Get the process name for this point.
    G4String GetProcessName() const { return fProcessName; }

    /// Get the energy deposit by this process for this point.  This is NOT
    /// enough information to calculate the total energy deposit for the
    /// track, but can be used to help decide which trajectory points are
    /// interesting to save to the output file.
    G4double GetProcessDeposit() const { return fProcessDeposit; }

    /// The name of the physical volume containing the stopping point of the
    /// current step.  This may (often) be a different volume than the volume
    /// referenced by GetVolumeNode().
    G4String GetPhysVolName() const { return fPhysVolName; }

    /// Get the node for the volume containing the stopping point.  If the
    /// stopping point is on a geometric boundary, this is the volume that the
    /// track is just exiting. 
    int GetVolumeNode() const;

    // Get method for HEPRep style attributes

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    
//---------
  private:
//---------

// Member data
    
    G4double fTime;
    G4ThreeVector fMomentum;
    G4StepStatus fStepStatus;
    G4ProcessType fProcessType;
    G4String fProcessName;
    G4double fProcessDeposit;
    G4String fPhysVolName;
    G4ThreeVector fPrevPosition;
};

#if defined G4TRACKING_ALLOC_EXPORT
extern G4DLLEXPORT G4Allocator<DSimTrajectoryPoint> aDSimTrajPointAllocator;
#else
extern G4DLLIMPORT G4Allocator<DSimTrajectoryPoint> aDSimTrajPointAllocator;
#endif

inline void* DSimTrajectoryPoint::operator new(size_t) {
    void *aTrajectoryPoint = (void *) aDSimTrajPointAllocator.MallocSingle();
    return aTrajectoryPoint;
}

inline void DSimTrajectoryPoint::operator delete(void *aTrajectoryPoint) {
    aDSimTrajPointAllocator.FreeSingle(
        (DSimTrajectoryPoint *) aTrajectoryPoint);
}
#endif

