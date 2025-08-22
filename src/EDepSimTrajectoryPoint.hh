#ifndef EDepSim_TrajectoryPoint_hh_seen
#define EDepSim_TrajectoryPoint_hh_seen

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
/// event is saved.  It keeps track of information like the momentum at the
/// point, the process that occured the the point, the volume containing the
/// point, and the energy deposited at the point.
namespace EDepSim {class TrajectoryPoint;}
class EDepSim::TrajectoryPoint : public G4TrajectoryPoint {
public:
    TrajectoryPoint();
    TrajectoryPoint(const G4Track* aTrack);
    TrajectoryPoint(const G4Step* aStep);
    TrajectoryPoint(const EDepSim::TrajectoryPoint &right);
    virtual ~TrajectoryPoint();

    inline void *operator new(size_t);
    inline void operator delete(void *aTrajectoryPoint);
    inline int operator==(const EDepSim::TrajectoryPoint& right) const
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

    /// Get the process sub type for this point.  The sub-types depend on the
    /// specific processes and are defined in a few different include files.
    /// For EM and hadronic interacitons, see G4EmProcessSubType.hh, and
    /// G4HadronicProcessType.hh.
    G4int GetProcessSubType() const { return fProcessSubType; }

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

    // Return Material 
    G4String GetMaterial() const { return fStepMaterial; }

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
    G4int fProcessSubType;
    G4String fProcessName;
    G4double fProcessDeposit;
    G4String fPhysVolName;
    G4ThreeVector fPrevPosition;
    G4String fStepMaterial;
};

#if defined G4TRACKING_ALLOC_EXPORT
extern G4DLLEXPORT G4Allocator<EDepSim::TrajectoryPoint> aTrajPointAllocator;
#else
extern G4DLLIMPORT G4Allocator<EDepSim::TrajectoryPoint> aTrajPointAllocator;
#endif

inline void* EDepSim::TrajectoryPoint::operator new(size_t) {
    void *aTrajectoryPoint = (void *) aTrajPointAllocator.MallocSingle();
    return aTrajectoryPoint;
}

inline void EDepSim::TrajectoryPoint::operator delete(void *aTrajectoryPoint) {
    aTrajPointAllocator.FreeSingle(
        (EDepSim::TrajectoryPoint *) aTrajectoryPoint);
}
#endif

