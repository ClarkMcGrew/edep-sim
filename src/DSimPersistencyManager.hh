// Provide output the Geant4 way.
// 
#ifndef DSimPersistencyManager_h
#define DSimPersistencyManager_h 1

#include <vector>

#include "G4VPersistencyManager.hh"
#include <G4StepStatus.hh>

class G4Event;
class G4Run;
class G4VPhysicalVolume;
class G4VTrajectory;
class G4TrajectoryContainer;

class DSimPersistencyMessenger;

class TPRegexp;

///   The class is `singleton', with access via
///   G4VPersistencyManager::GetPersistencyManager().
class DSimPersistencyManager : public G4VPersistencyManager {
public:
    virtual ~DSimPersistencyManager();

    /// stores anEvent and the associated objects into database.
    virtual G4bool Store(const G4Event* anEvent);
    virtual G4bool Store(const G4Run* aRun);
    virtual G4bool Store(const G4VPhysicalVolume* aWorld);
    
    virtual G4bool Retrieve(G4Event *&aEvent) {return false;}
    virtual G4bool Retrieve(G4Run*& aRun) {return false;}
    virtual G4bool Retrieve(G4VPhysicalVolume*& aWorld) {return false;};
    
    /// Open the output (ie database) file.
    virtual G4bool Open(G4String filename) = 0;
    
    /// Return the output file name.
    virtual G4String GetFilename(void) const {return fFilename;}
    
    /// Make sure the output file is closed.
    virtual G4bool Close(void) = 0;

    /// Set the threshold for length in a sensitive detector above which a
    /// trajectory will be saved.  If a trajectory created this much track
    /// inside a sensitive detector, then it is saved.
    virtual void SetLengthThreshold(G4double thresh) {
        fLengthThreshold = thresh;
    }

    /// Get the threshold for length in a sensitive detector.
    virtual G4double GetLengthThreshold() const {
        return fLengthThreshold;
    }

    /// Set the momentum threshold required to save a gamma-ray as a
    /// trajectory.
    virtual void SetGammaThreshold(G4double thresh) { 
        fGammaThreshold = thresh; 
    }

    /// Get the momentum threshold required to save a gamma-ray as a
    /// trajectory.  Gamma rays are rejected if the gamma-ray momentum is
    /// below either GetGammaThreshold() or GetMomentumThreshold().
    virtual G4double GetGammaThreshold() const {
        return fGammaThreshold;
    }

    /// Set the momentum threshold required to save a neutron as a
    /// trajectory.
    virtual void SetNeutronThreshold(G4double thresh) { 
        fNeutronThreshold = thresh; 
    }

    /// Get the momentum threshold required to save a neutron as a
    /// trajectory.
    virtual G4double GetNeutronThreshold() const {
        return fNeutronThreshold;
    }

    /// Get the required trajectory accuracy.  Trajectory points are saved so
    /// that the interpolated position of a trajectory is never more than this
    /// distance from the unsaved points.
    virtual void SetTrajectoryPointAccuracy(double acc) {
        fTrajectoryPointAccuracy = acc;
    }

    /// Get the required trajectory accuracy.  Trajectory points are saved so
    /// that the interpolated position of a trajectory is never more than this
    /// distance from the unsaved points.
    virtual double GetTrajectoryPointAccuracy(void) const {
        return fTrajectoryPointAccuracy;
    }

    /// Set the flag to save primary particle trajectories.  If this flag is
    /// true, then the trajectories for primary particles are saved even if
    /// they don't ultimately cause an energy deposition in a sensitive
    /// detector.  If this flag is false, the trajectories for primary
    /// particles are only saved if they, or one of their children, deposit
    /// energy in a sensitive detector.
    virtual void SetSaveAllPrimaryTrajectories(bool val) {
        fSaveAllPrimaryTrajectories = val;
    }

    /// Get the flag to save primary particle trajectories.  If this flag is
    /// true, then the trajectories for primary particles are saved even if
    /// they don't ultimately cause an energy deposition in a sensitive
    /// detector.  If this flag is false, the trajectories for primary
    /// particles are only saved if they, or one of their children, deposit
    /// energy in a sensitive detector.
    virtual bool GetSaveAllPrimaryTrajectories(void) const {
        return fSaveAllPrimaryTrajectories;
    }


    /// Add a regular expression to define a volume boundary at which a
    /// trajectory point should be saved.  The volume names are defined by the
    /// constructors which built the volume.  Most of the names can be found
    /// by looking at the geant command list.  The names are not the same as
    /// the root volume names.
    virtual void AddTrajectoryBoundary(const G4String& boundary);

    /// Clear the list of volume boundaries which will cause a trajectory
    /// point to be saved.
    virtual void ClearTrajectoryBoundaries();

    /// Set the detector mask.
    void SetDetectorPartition(int partition) {fDetectorPartition = partition;}

    /// Get the detector partition.
    int GetDetectorPartition() const {return fDetectorPartition;}
    
protected:
    /// This is protected so that it doesn't get called outside of the class
    /// implementation.  Use GetPersistencyManager instead.
    DSimPersistencyManager(); 

    /// Set the filename
    void SetFilename(G4String file) {
        fFilename = file;
    }

    /// Return the intensity used to generate the events.  This is a
    /// convenience function that accesses the intensity from the run manager.
    /// The intensity should be saved in the MC header for the event.
    // double void GetIntensity() const;

    /// Mark the G4 Trajectories that should be saved.
    void MarkTrajectories(const G4Event* event);

    /// Find the maximum deviation of a trajectory point from the interpolated
    /// path between point 1 and point 2
    double FindTrajectoryAccuracy(G4VTrajectory* traj, int point1, int point2);

    /// Save another trajectory point between point 1 and point to optimize
    /// the trajectory accuracy.
    int SplitTrajectory(G4VTrajectory* traj, int point1, int point2);

    /// Fill a vector with the indices of trajectory points that should be
    /// copied to the output file.
    void SelectTrajectoryPoints(std::vector<int>& selected, 
                                G4VTrajectory* g4Traj);
    
    /// Return true if a trajectory point should be saved.  The decision is
    /// based on the stepping status (must be fGeomBoundary), the current and
    /// the previous volume name (one must match a trajectory boundary regexp,
    /// the other must not).
    bool SaveTrajectoryBoundary(G4VTrajectory* g4Traj,
                                G4StepStatus status,
                                G4String currentVolume,
                                G4String prevVolume);

private: 
    /// The filename of the output file.
    G4String fFilename;

    /// If a trajectory left this much track in a sensitive detector then it
    /// should be saved.
    G4double fLengthThreshold;

    /// Threshold momentum above which gamma-ray trajectories are written.
    /// Gamma-rays are rejected if the momentum is less than either
    /// fGammaThreshold or fMomentumThreshold.
    G4double fGammaThreshold;

    /// The threshold momentum below which a neutron will be rejected.
    G4double fNeutronThreshold;

    /// The maximum distance between the interpolated trajectory position and
    /// an unsaved trajectory point.
    double fTrajectoryPointAccuracy;

    /// Flag to determine if all primary trajectories are saved, or only those
    /// that ultimately create energy in a sensitive detector.  The primary
    /// particles are always saved.
    bool fSaveAllPrimaryTrajectories;

    /// A list of detectors for which trajectory points should be saved as
    /// particles enter and exit.
    std::vector<TPRegexp*> fTrajectoryBoundaries;

    /// The detector partition
    int fDetectorPartition;
    
    // A pointer to the messenger.
    DSimPersistencyMessenger* fPersistencyMessenger;
};
#endif
