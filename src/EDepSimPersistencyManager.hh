// Provide output the Geant4 way.
// 
#ifndef EDepSim_PersistencyManager_h
#define EDepSim_PersistencyManager_h 1

#include "TG4Event.h"

#include <G4VPersistencyManager.hh>
#include <G4StepStatus.hh>

#include <vector>
#include <map>

class G4Event;
class G4Run;
class G4PrimaryVertex;
class G4VPhysicalVolume;
class G4VTrajectory;
class G4TrajectoryContainer;
class G4VHitsCollection;

class TPRegexp;

namespace EDepSim {class PersistencyMessenger;}

/// The class is `singleton', with access via
/// G4VPersistencyManager::GetPersistencyManager().  You need to create a
/// single persistency manager for GEANT4 to work right.  It must derive from
/// G4VPersistencyManager which will make this object available to the
/// G4RunManager as a singleton.  As a singleton, there can only be one
/// persistency manager active at a time.  The Store methods will then be
/// called by the G4RunManager::AnalyzeEvent method (the
/// G4RunManager::AnalyzeMethod is protected).
///
/// The persistency manager doesn't *have* to be derived from
/// EDepSim::RootPersistencyManager, but a lot of the trajectory and hit handling
/// functionality is currently handled by that class, and as of 4.10.2,
/// persistency managers seem to be the only viable hook to allow the event to
/// be summarized after it is fully generated.  An alternative place to
/// summarize the information would be in the
/// G4UserEventAction::EndOfEventAction method and then keep the summary
/// data in the EventUserInformation which is thread local.
namespace EDepSim {class PersistencyManager;}
class EDepSim::PersistencyManager : public G4VPersistencyManager {
public:
    /// Creates a root persistency manager.  Through the "magic" of
    /// G4VPersistencyManager the ultimate base class, this declared to the G4
    /// persistency management system.  You can only have one active
    /// persistency class at any give moment.
    PersistencyManager();
    virtual ~PersistencyManager();

    /// stores anEvent and the associated objects into database.
    virtual G4bool Store(const G4Event* anEvent);
    virtual G4bool Store(const G4Run* aRun);
    virtual G4bool Store(const G4VPhysicalVolume* aWorld);
    
    /// Retrieve information from a file.  These are not implemented.
    virtual G4bool Retrieve(G4Event *&e) {e=NULL; return false;}
    virtual G4bool Retrieve(G4Run* &r) {r=NULL; return false;}
    virtual G4bool Retrieve(G4VPhysicalVolume* &w) {w=NULL; return false;}
    
    /// A public accessor to the summarized event.  The primaries are
    /// summarized during by a call to UpdateSummaries.  The
    /// EDepSim::PersistencyManager::Store(event) method is called from
    /// G4RunManager::AnalyzeEvent and will call the UpdateSummaries() method.
    /// Alternatively, the UpdateSummarize method can be called by a method of
    /// a derived class (e.g. in its Store method).  The fEventSummary field
    /// is protected so derived classes can directly access it.
    const TG4Event& GetEventSummary();

    /// A public accessor to the summarized primaries.  The primaries are
    /// summarized during by a call to UpdateSummaries.  The
    /// EDepSim::PersistencyManager::Store(event) method is called from
    /// G4RunManager::AnalyzeEvent and will call the UpdateSummaries() method.
    /// Alternatively, the UpdateSummarize method can be called by a method of
    /// a derived class (e.g. in its Store method).
    const std::vector<TG4PrimaryVertex>& GetPrimaries() const;

    /// A public accessor to the summarized trajectories.  See the
    /// documentation for the GetPrimaries() method.
    const std::vector<TG4Trajectory>& GetTrajectories() const;

    /// A public accessor to the summarized hit segment detectors.  See the
    /// documentation for the GetPrimaries() method.
    const TG4HitSegmentDetectors& GetSegmentDetectors() const;

    /// Open the output (ie database) file.  This is used by the persistency
    /// messenger to open files using the G4 macro language.  It can be an
    /// empty method.
    virtual G4bool Open(G4String filename);
    
    /// Make sure the output file is closed.  This is used to make sure that
    /// any information being summarized has been saved.
    virtual G4bool Close(void);

    /// Return the output file name.
    virtual G4String GetFilename(void) const {return fFilename;}
    
    /// Set the threshold for length in a sensitive detector above which a
    /// trajectory will be saved.  If a trajectory created this much track
    /// inside a sensitive detector, then it is saved.
    virtual void SetLengthThreshold(G4double thresh) {
        fLengthThreshold = thresh;
    }

    /// Get the threshold for length in a sensitive detector.
    virtual G4double GetLengthThreshold() const {return fLengthThreshold;}

    /// Set the momentum threshold required to save a gamma-ray as a
    /// trajectory.
    virtual void SetGammaThreshold(G4double thresh) { 
        fGammaThreshold = thresh; 
    }

    /// Get the momentum threshold required to save a gamma-ray as a
    /// trajectory.  Gamma rays are rejected if the gamma-ray momentum is
    /// below either GetGammaThreshold() or GetMomentumThreshold().
    virtual G4double GetGammaThreshold() const {return fGammaThreshold;}

    /// Set the momentum threshold required to save a neutron as a
    /// trajectory.
    virtual void SetNeutronThreshold(G4double thresh) { 
        fNeutronThreshold = thresh; 
    }

    /// Get the momentum threshold required to save a neutron as a
    /// trajectory.
    virtual G4double GetNeutronThreshold() const {return fNeutronThreshold;}

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
    /// Set the output filename.  This can be used by the derived classes to
    /// inform the base class of the output file name.
    void SetFilename(G4String file) {
        fFilename = file;
    }

    /// Update the event summary fields.
    void UpdateSummaries(const G4Event* event);

    /// A summary of the primary vertices in the event.
    TG4Event fEventSummary;

private: 
    // Fill the vertex container.  This allows informational vertices to be
    // filled.
    void SummarizePrimaries(std::vector<TG4PrimaryVertex>& primaries,
                            const G4PrimaryVertex* event);

    /// Fill the trajectory container.
    void SummarizeTrajectories(std::vector<TG4Trajectory>& trajectories,
                               const G4Event* event);
    
    /// Mark the G4 Trajectories that should be saved.
    void MarkTrajectories(const G4Event* event);

    /// sensitive detector.
    void SummarizeSegmentDetectors(TG4HitSegmentDetectors& segmentDetectors,
                                   const G4Event* event);

    /// Fill a container of hit segments.
    void SummarizeHitSegments(std::vector<TG4HitSegment>& segments,
                              G4VHitsCollection* hits);

    /// Fill the map of sensitive detectors which use hit segments as the
    /// Copy and map the contributing trajectories from the vector of
    /// contributors for the EDepSim::HitSegment into the vector of contributors
    /// for the TG4HitSegment.
    void CopyHitContributors(std::vector<int>& dest,
                             const std::vector<int>& src);

    /// Copy the trajectory points into the trajectory summary.  This uses
    /// SelectTrajectoryPoints to pick out the points that should appear in
    /// the summary.
    void CopyTrajectoryPoints(TG4Trajectory& traj,
                              G4VTrajectory* g4Traj);

    /// Find the maximum deviation of a trajectory point from the interpolated
    /// path between point 1 and point 2
    double FindTrajectoryAccuracy(G4VTrajectory* traj,
                                  int point1, int point2);

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

    /// The mapping between the internal G4 TrackID number and the external
    /// TrackId number.  The G4 TrackID value is based on the order that the
    /// particle is placed onto the stack, not all TrackID value exist and the
    /// ordering in the vector is not monotonic.  The external TrackId is the
    /// same as the location of the trajectory in the output array and can be
    /// used as an index so parent trajectories are "easy" to find.  This is
    /// filled in SummarizeTrajectories, and used to remap TrackId in other
    /// methods.
    typedef std::map<int,int> TrackIdMap;
    TrackIdMap fTrackIdMap;

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
    EDepSim::PersistencyMessenger* fPersistencyMessenger;
};
#endif
