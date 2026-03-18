#ifndef TG4PhotonHit_h_seen
#define TG4PhotonHit_h_seen

#include <TObject.h>
#include <TLorentzVector.h>

#include <map>
#include <string>
#include <vector>

namespace EDepSim {class PersistencyManager;}
class TG4PhotonHit;

/// A container for the hit segment information.
typedef std::vector<TG4PhotonHit> TG4PhotonHitContainer;

/// A map with one entry per sensitive detector using the TG4PhotonHit object
/// to summarize hits.
typedef std::map<std::string,TG4PhotonHitContainer> TG4PhotonHitDetectors;

/// A single optical-photon in the detector.
class TG4PhotonHit : public TObject {
     friend class EDepSim::PersistencyManager;
public:
    TG4PhotonHit()
        : Stop(0,0,0,0), Start(0,0,0,0),
          PrimaryId(-1), Process(-1),
          EnergyDeposit(0) {}
    virtual ~TG4PhotonHit();

    /// The track id of the particle that created this photon.  If the track
    /// id was not available, this will be negative one. Note: This is only
    /// guaranteed to be available with savePhotonTrajectories is true and
    /// photons are directly tracked by G4.  If the photons are offloaded it
    /// probably is not available.
    int GetPrimaryId() const {return PrimaryId;}

    /// Get the energy deposit for this photon
    double GetEnergyDeposit() const {return EnergyDeposit;}

    /// Get the wave length for this photon (i.e. hc/energy)
    double GetWavelength() const {return 1.23984e-09/EnergyDeposit;}

    /// Get the position the photon was stopped.  This will be on the surface
    /// of the sensitive detector that detected the photon.
    const TLorentzVector& GetPosition() const {return Stop;}

    /// An alternate name for the photon position for consistency with
    /// TG4HitSegment
    const TLorentzVector& GetStop() const {return Stop;}

    /// Get the starting position for the photon.  This may not be available
    /// if photon tracking is offloaded.
    const TLorentzVector& GetStart() const {return Start;}

    /// Get the process type that created the photon.  The expected values are
    /// 0) Cherenkov, 2) Scintillation, and 7) Wave length shifter.
    int GetProcess() const {return Process;}

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
    /// Absorption point for the photon.
    TLorentzVector Stop;

    /// The creation point for the photon.
    TLorentzVector Start;

    /// The track id of the particle that created this photon.  This will be
    /// negative one if the track information is not available for this
    /// photon.
    int PrimaryId;

    /// The process type that created the photon.  The expected values
    /// are 0) Cherenkov, 2) Scintillation, and 7) Wave length shifter.
    int Process;

    /// Photon energy.
    Float_t EnergyDeposit;

    ClassDef(TG4PhotonHit, 1)
};

#endif
