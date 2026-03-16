////////////////////////////////////////////////////////////

#ifndef EDepSim_HitSurface_h
#define EDepSim_HitSurface_h

#include <vector>

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

class G4Step;
class G4Track;

#include "EDepSimVolumeId.hh"

/// This is a base class used to save G4Hit objects into a root output file.
/// It contains the global position of the hit on the surface of a detector,
/// plus the any fields provided by CP::TG4VHit.  The electronics response is
/// simulated in a separate program (library) which will construct the
/// digitized hits.  This class is used with the EDepSim::SurfaceSD sensitive
/// detector class, and can be requested using the "surface" name from
/// EDepSim::SensitiveDetectorFactor::MakeSD().
namespace EDepSim {class HitSurface;}
class EDepSim::HitSurface : public G4VHit {
public:
    /// Create a new hit surface
    HitSurface();

    HitSurface(const EDepSim::HitSurface& rhs);
    virtual ~HitSurface();

    typedef G4THitsCollection<EDepSim::HitSurface> HitSurfaceCollection;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    /// Disambiguate  the draw and  print methods  to distinguish  between the
    /// G4VHit and  TObject definitions.
    virtual void Draw();
    virtual void Print();
    virtual void Draw(const char*) {}
    virtual void Print(const char*) const {}

    /// Provide public access to the contributors for internal G4 classes.
    std::vector<int>& GetContributors() {return fContributors;}

    /// Return a list of track identifiers that contributed to this hit.
    /// These track ids can be used as indicies to find trajectories in the
    /// TG4TrajectoryContainer object associated with an MC event.
    int GetContributor(int i) const {
        return fContributors[i];
    }

    /// Return a list of track identifiers that contributed to this hit.
    /// These track ids can be used as indicies to find trajectories in the
    /// TG4TrajectoryContainer object associated with an MC event.
    int GetContributorCount() const {
        return fContributors.size();
    }

    /// Get the TrackId of the "primary" particle that is associated with this
    /// hit.  For a photon, this will be the track that created the photon,
    /// not the photon. This is slightly complicated since the "interesting"
    /// primary particle is saved.  For instance, if the primary particle is a
    /// pizero, The TrackId of the gamma-rays from the decay is saved.
    /// Likewise, the electron from a muon decay is used as the primary
    /// particle.  You can find the "really truly" primary particle by finding
    /// the trajectory associated with this PrimaryId, and then working
    /// backwards to the associated G4PrimaryParticle (You can tell that a
    /// trajectory comes from a primary particle by checking if it's ParentID
    /// is zero.  If it is zero, the trajectory came from a primary).
    int GetPrimaryTrajectoryId(void) const {return fPrimaryId;}

    /// Get the total energy deposited in this hit.
    double GetEnergyDeposit(void) const {return fEnergyDeposit;}

    /// The position of hit.
    const G4LorentzVector& GetPosition() const {return fPosition;}

    /// The starting position of the track (usually a photon) that made this
    /// hit.
    const G4LorentzVector& GetStart() const {return fStart;}

    /// Print the hit information.
    void ls(std::string = "") const;

protected:
    /// Find the primary track ID for the current track.  This is the primary
    /// that is the ultimate parent of the current track.
    int FindPrimaryId(G4Track* theTrack);

private:

    /// The TrackID for each trajectory that contributed to this hit.  This
    /// could contain the TrackID of the primary particle, but not
    /// necessarily.
    std::vector<int> fContributors;

    /// The track id of the primary particle.
    int fPrimaryId;

    /// The total energy deposit in this hit.  For a photon, this is the
    /// photon energy.
    double fEnergyDeposit;

    /// The position of the surface hit.
    G4LorentzVector fPosition;

    /// The starting position of the track (i.e. the photon) that made the
    /// hit.
    G4LorentzVector fStart;

    /// The G4 physical volume that contains the hit.
    EDepSim::VolumeId fHitVolume;

};

extern G4Allocator<EDepSim::HitSurface> edepHitSurfaceAllocator;

inline void* EDepSim::HitSurface::operator new(size_t) {
    void *aHit;
    aHit = (void *) edepHitSurfaceAllocator.MallocSingle();
    return aHit;
}

inline void EDepSim::HitSurface::operator delete(void *aHit) {
    edepHitSurfaceAllocator.FreeSingle((EDepSim::HitSurface*) aHit);
}

#endif
