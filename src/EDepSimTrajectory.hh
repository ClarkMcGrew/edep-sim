#ifndef EDepSim_Trajectory_hh_seen
#define EDepSim_Trajectory_hh_seen

#include <stdlib.h>
#include <vector>

#include <globals.hh>
#include <G4VTrajectory.hh>
#include <G4Allocator.hh>
#include <G4ios.hh>
#include <G4ParticleDefinition.hh>
#include <G4Track.hh>
#include <G4Step.hh>

#include "EDepSimTrajectoryPoint.hh"

typedef std::vector<G4VTrajectoryPoint*>  TrajectoryPointContainer;

namespace EDepSim {class Trajectory;}

/// EDepSim specific trajectory class to save internal bookkeeping
/// information.  This keeps track of information about a particular particle
/// track.  Important information kept includes how much energy this
/// trajectory has deposited in sensitive detectors, and it can query it's
/// children to find out how much energy they deposited.  It also keeps track
/// of the process that created the trajectory, the total length in the
/// sensitive detectors, and points along the trajectory.
class EDepSim::Trajectory : public G4VTrajectory {
public:
    Trajectory();
    
    Trajectory(const G4Track* aTrack);
    Trajectory(EDepSim::Trajectory &);
    virtual ~Trajectory();
    
    inline void* operator new(size_t);
    inline void  operator delete(void*);
    inline int operator == (const EDepSim::Trajectory& right) const {
        return (this==&right);
    } 

    /// Get the track id described by this trajectory.
    inline G4int GetTrackID() const {return fTrackID;}

    /// Get the track id of the parent of the track described by this
    /// trajectory.
    inline G4int GetParentID() const {return fParentID;}

    /// Get the particle name.
    inline G4String GetParticleName() const {return fParticleName;}

    /// Get the particle charge.
    inline G4double GetCharge() const {return fPDGCharge;}

    /// Get the PDG MC particle number for this particle.
    inline G4int GetPDGEncoding() const {return fPDGEncoding;}

    /// Get the interaction process that created the trajectory.
    inline G4String GetProcessName() const {return fProcessName;}

    /// Get the initial momentum of the particle that created this trajectory.
    inline G4ThreeVector GetInitialMomentum() const {return fInitialMomentum;}
    
    /// Get the initial kinetic energy of the particle that created this
    /// trajectory.
    G4double GetInitialKineticEnergy() const;

    /// Get the amount of energy deposited into a sensitive detector.
    G4double GetSDEnergyDeposit() const {return fSDEnergyDeposit;}

    /// Add energy deposited into a sensitive detector.
    void AddSDEnergyDeposit(double energy) {
        fSDEnergyDeposit += energy;
        fSDTotalEnergyDeposit += energy;
    }

    /// Get the total length of this trajectory that is in a sensitive detector.
    G4double GetSDLength() const {return fSDLength;}

    /// Add the length that has been deposited into a sensitive detector.
    void AddSDLength(double len) {
        fSDLength += len;
    }

    /// Get the total amount of energy deposited into a sensitive detector by
    /// this trajectory and all of it's daughters.
    G4double GetSDTotalEnergyDeposit() const {return fSDTotalEnergyDeposit;}

    /// Add energy deposited into a sensitive detector by a daughter.
    void AddSDDaughterEnergyDeposit(double energy) {
        fSDTotalEnergyDeposit += energy;
    }

    /// Get the range of the particle that created this trajectory.
    G4double GetRange() const;

    /// Mark this trajectory as one that should be saved in the output.
    void MarkTrajectory(bool save=true);

    /// Check if this trajectory should be saved.
    bool SaveTrajectory() const { return fSaveTrajectory;}

    virtual void AppendStep(const G4Step* aStep);

    /// Get the number of trajectory points saved with this trajectory.
    virtual int GetPointEntries() const {return fPositionRecord->size();}

    /// Get a particular trajectory point.
    virtual G4VTrajectoryPoint* GetPoint(G4int i) const {
        return (*fPositionRecord)[i];
    }
    
    virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

    /// Get the full definition of the particle.
    G4ParticleDefinition* GetParticleDefinition() const;
    
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    
private:
    
    TrajectoryPointContainer* fPositionRecord;
    G4int                     fTrackID;
    G4int                     fParentID;
    G4int                     fPDGEncoding;
    G4double                  fPDGCharge;
    G4String                  fParticleName;
    G4String                  fProcessName;
    G4ThreeVector             fInitialMomentum;
    G4double                  fSDEnergyDeposit;
    G4double                  fSDTotalEnergyDeposit;
    G4double                  fSDLength;
    bool                      fSaveTrajectory;
};

#if defined G4TRACKING_ALLOC_EXPORT
extern G4DLLEXPORT G4Allocator<EDepSim::Trajectory> aTrajAllocator;
#else
extern G4DLLIMPORT G4Allocator<EDepSim::Trajectory> aTrajAllocator;
#endif

inline void* EDepSim::Trajectory::operator new(size_t) {
    void* aTrajectory = (void*) aTrajAllocator.MallocSingle();
    return aTrajectory;
}

inline void EDepSim::Trajectory::operator delete(void* aTrajectory) {
    aTrajAllocator.FreeSingle((EDepSim::Trajectory*)aTrajectory);
}
#endif
