////////////////////////////////////////////////////////////
// $Id: EDepSim::HitSegment.hh,v 1.5 2011/06/29 04:35:53 mcgrew Exp $

#ifndef EDepSim_HitSegment_h
#define EDepSim_HitSegment_h

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
/// It contains the global position of the starting point and stopping point
/// of the track segment that created the hit, plus the any fields provided by
/// CP::TG4VHit.  The electronics response is simulated in a separate program
/// (library) which will construct the digitized hits.  This class is used
/// with the EDepSim::SegmentSD sensitive detector class, and can be requested
/// using the "segment" name from EDepSim::SensitiveDetectorFactor::MakeSD().
namespace EDepSim {class HitSegment;}
class EDepSim::HitSegment : public G4VHit {
public:
    /// Create a new hit segment with a maximum allowed sagitta and length.
    /// The default values are set so that normally, a scintillator element
    /// will only have a single hit for a through going track (& delta-rays).
    HitSegment(double maxSagitta = 1*CLHEP::mm,
               double maxLength = 5*CLHEP::mm);

    HitSegment(const EDepSim::HitSegment& rhs);
    virtual ~HitSegment();

    typedef G4THitsCollection<EDepSim::HitSegment> HitSegmentCollection;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    /// Add the effects of a part of a step to this hit.
    virtual void AddStep(G4Step* theStep);

    /// Hits for the same primary particle, in the same physical volume belong
    /// in the same hit.
    virtual bool SameHit(G4Step* theStep);

    /// Disambiguate  the draw and  print methods  to distinguish  between the
    /// G4VHit and  TObject definitions.
    virtual void Draw();
    virtual void Print();
    virtual void Draw(const char*) {}
    virtual void Print(const char*) const {}

    /// Provide public access to the contributors for internal G4 classes.
    std::vector<int>& GetContributors() {return fContributors;}

    /// Find the distance from the starting point to stoping point of the
    /// track.
    virtual double GetLength() const;

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
    /// hit.  This is slightly complicated since the "interesting" primary
    /// particle is saved.  For instance, if the primary particle is a pizero,
    /// The TrackId of the gamma-rays from the decay is saved.  Likewise, the
    /// electron from a muon decay is used as the primary particle.  You can
    /// find the "really truly" primary particle by finding the trajectory
    /// associated with this PrimaryId, and then working backwards to the
    /// associated G4PrimaryParticle (You can tell that a trajectory comes
    /// from a primary particle by checking if it's ParentID is zero.  If it
    /// is zero, the trajectory came from a primary).
    int GetPrimaryTrajectoryId(void) const {return fPrimaryId;}

    /// Get the total energy deposited in this hit.
    double GetEnergyDeposit(void) const {return fEnergyDeposit;}

    /// Get the secondary energy deposited in this hit (see the field
    /// documentation).
    double GetSecondaryDeposit(void) const {return fSecondaryDeposit;}

    /// Get the total charged track length in this hit.  This includes all of
    /// the contributions from secondary particles that got lumped into this
    /// hit (e.g. the contributions from delta-rays).
    double GetTrackLength(void) const {return fTrackLength;}

    /// Get the variance of the energy loss distribution along the track length
    double GetEnergyVariance(void) const;

    /// The position of the starting point.
    const G4LorentzVector& GetStart() const {return fStart;}

    /// The position of the stopping point.
    const G4LorentzVector& GetStop() const {return fStop;}

#ifdef BOGUS
    /// The X position of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartX(void) const {return fStartX;}

    /// The Y position of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartY(void) const {return fStartY;}

    /// The Z position of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartZ(void) const {return fStartZ;}

    /// The time of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartT(void) const {return fStartT;}

    /// The X position of the hit stoping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopX(void) const {return fStopX;}

    /// The Y position of the hit stoping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopY(void) const {return fStopY;}

    /// The Z position of the hit stoping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopZ(void) const {return fStopZ;}

    /// The time of the hit stopping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopT(void) const {return fStopT;}
#endif

    /// Print the hit information.
    void ls(std::string = "") const;

protected:
    /// Find the primary track ID for the current track.  This is the primary
    /// that is the ultimate parent of the current track.
    int FindPrimaryId(G4Track* theTrack);

    /// Find the maximum separation (the sagitta) between the current hit
    /// segment path points, and the straight line connecting the start and
    /// proposed new stop point.
    double FindSagitta(G4Step* theStep);

    /// Find the maximum distance from the hit segment to the new step that is
    /// proposed to be added to the hit segment. This is used to
    /// combine secondaries with a parent track.
    double FindSeparation(G4Step* theStep);

private:

    /// The sagitta tolerance for the segment.
    double fMaxSagitta;

    /// The maximum length between the start and stop points of the segment.
    double fMaxLength;

    /// The TrackID for each trajectory that contributed to this hit.  This
    /// could contain the TrackID of the primary particle, but not
    /// necessarily.
    std::vector<int> fContributors;

    /// The track id of the primary particle.
    int fPrimaryId;

    /// The total energy deposit in this hit.
    double fEnergyDeposit;

    /// The "secondary" energy deposit in this hit.  This is used to help
    /// simulate the recombination of electrons, and is part of the total
    /// energy deposit.
    ///
    /// DETSIM is expected to use this field to save the amount of energy
    /// deposited as opticalphotons.  The remaining energy will be deposited
    /// as ionization.  In this model (in argon), the mean number of quanta
    /// created will be <N_q> = (fEnergyDeposit)/(19.5*eV), N_q should be
    /// fluctuated around <N_q>, N_ph = N_q*fSecondaryDeposit/fEnergyDeposit,
    /// and N_e = N_q - N_ph.  Thd fSecondaryDeposit value already includes
    /// the binomial fluctuation, so don't fluctuate N_ph or N_e.
    double fSecondaryDeposit;

    /// The total charged track length in this hit.  This includes the
    /// contribution from all of the secondary particles (e.g. delta-rays)
    /// that are included in this hit.
    double fTrackLength;

    /// A sum needed to calculate the variance of the energy loss along the
    /// segment.  This is the sum of (delta E)^2/(delta L)
    double fEnergyDepositVarianceTerm;

    /// The starting position of the segment.
    G4LorentzVector fStart;

    /// The stopping position of the segment.
    G4LorentzVector fStop;

private:
    /// The G4 physical volume that contains the hit.
    EDepSim::VolumeId fHitVolume;

    /// The end points of the steps that make up this hit.  This is used to
    /// make sure that the current hit stays inside of it's allowed
    /// tolerances.
    std::vector<G4ThreeVector> fPath;

};

extern G4Allocator<EDepSim::HitSegment> edepHitSegmentAllocator;

inline void* EDepSim::HitSegment::operator new(size_t) {
    void *aHit;
    aHit = (void *) edepHitSegmentAllocator.MallocSingle();
    return aHit;
}

inline void EDepSim::HitSegment::operator delete(void *aHit) {
    edepHitSegmentAllocator.FreeSingle((EDepSim::HitSegment*) aHit);
}

#endif
