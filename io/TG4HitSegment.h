#ifndef TG4HitSegment_hxx_seen
#define TG4HitSegment_hxx_seen

#include <TLorentzVector.h>
#include <TObject.h>

#include <map> 
#include <vector> 

namespace EDepSim {class PersistencyManager;}
class TG4HitSegment;

/// A container for the hit segment information.
typedef std::vector<TG4HitSegment> TG4HitSegmentContainer;

/// A map with one entry per sensitive detector using the TG4HitSegment object
/// to summarize hits.
typedef std::map<std::string,TG4HitSegmentContainer> TG4HitSegmentDetectors;

/// Save the amount and location of energy deposition.  It contains the global
/// position of the starting point and stopping point of the track segment
/// that created the hit.  The energy should be assumed to be deposited
/// uniformly between the two points (the length of the segment can, and
/// should, be limited in G4, so that shouldn't be a bad assumption).  Both
/// the total and secondary energy deposition is saved.  The definition of the
/// secondary energy depends on the configuration of the simulation, but
/// generally, it refers to the amount of energy going into scintillation.
class TG4HitSegment : public TObject {
     friend class EDepSim::PersistencyManager;
public:
     typedef std::vector<Int_t> Contributors;
     
     TG4HitSegment() 
        : PrimaryId(0), EnergyDeposit(0), SecondaryDeposit(0),
          TrackLength(0), Start(0,0,0,0), Stop(0,0,0,0) {}
    virtual ~TG4HitSegment();
    
    /// The track id of the most important particle associated with this hit
    /// segment.
    int GetPrimaryId() const {return PrimaryId;}

    /// The total energy deposit in this hit.  
    double GetEnergyDeposit() const {return EnergyDeposit;}

    /// The "secondary" energy deposit in this hit. Generally, this is used to
    /// help simulate the amount of energy emitted as scintillation light,
    /// i.e. opticalphotons, and is part of the total energy deposit.  The
    /// remaining energy will be deposited as ionization.  In this model (in
    /// argon), the mean number of quanta created will be <N_q> =
    /// (fEnergyDeposit)/(19.5*eV), N_q should be fluctuated around <N_q>,
    /// N_ph = N_q*fSecondaryDeposit/fEnergyDeposit, and N_e = N_q - N_ph.
    /// Thd fSecondaryDeposit value already includes the binomial fluctuation,
    /// so don't fluctuate N_ph or N_e.
    double GetSecondaryDeposit() const {return SecondaryDeposit;}

    /// The total charged track length in this hit.  This includes the
    /// contribution from all of the secondary particles (e.g. delta-rays)
    /// that are included in this hit.
    double GetTrackLength() const {return TrackLength;}

    /// The starting position of the segment.
    const TLorentzVector& GetStart() const {return Start;}

    /// The stopping position of the segment.
    const TLorentzVector& GetStop() const {return Stop;}

    /// The TrackId for each trajectory that contributed to this hit.  This
    /// could contain the TrackId of the primary particle, but not
    /// necessarily.  
    Contributors Contrib;

// The public fields are deprecated but still supported by default in the
// current version.
#define EDEPSIM_USE_PUBLIC_FIELDS
    
#if defined(EDEPSIM_USE_PUBLIC_FIELDS)&&!defined(EDEPSIM_FORCE_PRIVATE_FIELDS)&&!defined(__CINT__)
public:
#ifdef EDEPSIM_WARN_PUBLIC_FIELDS
#warning Using deprecated public fields.  Please consider using the accessor.  For example, to access PrimaryId, use GetPrimaryId().
#endif
#else
private:
#endif
    
    /// The track id of the most important particle associated with this hit
    /// segment.
    Int_t PrimaryId;

    /// The total energy deposit in this hit.  
    Float_t EnergyDeposit;

    /// The "secondary" energy deposit in this hit. Generally, this is used to
    /// help simulate the amount of energy emitted as scintillation light,
    /// i.e. opticalphotons, and is part of the total energy deposit.  The
    /// remaining energy will be deposited as ionization.  In this model (in
    /// argon), the mean number of quanta created will be <N_q> =
    /// (fEnergyDeposit)/(19.5*eV), N_q should be fluctuated around <N_q>,
    /// N_ph = N_q*fSecondaryDeposit/fEnergyDeposit, and N_e = N_q - N_ph.
    /// Thd fSecondaryDeposit value already includes the binomial fluctuation,
    /// so don't fluctuate N_ph or N_e.
    Float_t SecondaryDeposit;

    /// The total charged track length in this hit.  This includes the
    /// contribution from all of the secondary particles (e.g. delta-rays)
    /// that are included in this hit.
    Float_t TrackLength;

    /// The starting position of the segment.
    TLorentzVector Start;

    /// The stopping position of the segment.
    TLorentzVector Stop;

    ClassDef(TG4HitSegment,1);
};
#endif
