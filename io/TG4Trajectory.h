#ifndef TG4Trajectory_hxx_seen
#define TG4Trajectory_hxx_seen

#include <string>
#include <vector>

#include <TVector3.h>
#include <TLorentzVector.h>
#include <TObject.h>

namespace EDepSim {class PersistencyManager;}
class TG4Trajectory;
class TG4TrajectoryPoint;

/// A container for all of the trajectory information.  The trajectory
/// container is indexed by the trajectory TrackId so that
/// (container[index].TrackId==index).  That means that you can access the
/// parent trajectory using container[traj.ParentId].
typedef std::vector<TG4Trajectory> TG4TrajectoryContainer;

/// A class to save a G4 trajectory into a root output file without linking to
/// geant.  A trajectory is the truth information about the path of a particle
/// through the G4 simulation. It saves the parent trajectory that generated
/// this particle, the initial momentum of the particle, and the path followed
/// by the particle in the detector.  
class TG4Trajectory : public TObject {
    friend class EDepSim::PersistencyManager;
public:
    typedef std::vector<TG4TrajectoryPoint> TrajectoryPoints;
     
    TG4Trajectory(void)
        : TrackId(-1), ParentId(-1), 
          Name("none"), PDGCode(0),
          InitialMomentum(0,0,0,0) {}

    virtual ~TG4Trajectory();

    /// The TrackId of this trajectory.
    int GetTrackId() const {return TrackId;}
    
    /// The unique Id of the parent trajectory (The TrackId of the parent).
    int GetParentId() const {return ParentId;}
    
    /// The name of the particle.
    const char* GetName() const {return Name.c_str();}

    /// The PDG encoding of the particle.
    int GetPDGCode() const {return PDGCode;}
    
    /// The initial momentum of the particle
    const TLorentzVector& GetInitialMomentum() const {return InitialMomentum;}

    /// The trajectory points for this trajectory.
    TrajectoryPoints Points;

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
    
    /// The TrackId of this trajectory.
    Int_t TrackId;
    
    /// The unique Id of the parent trajectory (The TrackId of the parent).
    Int_t ParentId;
    
    /// The name of the particle.
    std::string Name;

    /// The PDG encoding of the particle.
    Int_t PDGCode;
    
    /// The initial momentum of the particle
    TLorentzVector InitialMomentum;

    ClassDef(TG4Trajectory,1)
};


/// A class to save a G4 trajectory point into a root output file without
/// linking to geant.  The trajectory point is saved in a TG4Trajectory as a
/// way to record the path of a particle through the detector.  This is the
/// truth information about the particles which were tracked, but is not a
/// good record of the energy deposition.  Use the TG4HitSegment objects for a
/// record of the energy deposition.
class TG4TrajectoryPoint : public TObject {
    friend class EDepSim::PersistencyManager;
public:
    TG4TrajectoryPoint()
        : Position(0,0,0,0), Momentum(0,0,0),
          Process(0), Subprocess(0) {}
          
    virtual ~TG4TrajectoryPoint();

    /// Process types copied from the G4 definitions so that this can be
    /// compiled without having geant4 installed.  Check the exact definitions
    /// are in the geant4 documentation, but the (important) names are pretty
    /// self explanatory.  The definitions can be found in the geant4 include
    /// file named G4ProcessType.hh.
    enum G4ProcessType {
        kProcessNotDefined = 0,
        kProcessTransportation = 1,
        kProcessElectromagetic = 2 ,
        kProcessOptical = 3,
        kProcessHadronic = 4,
        kProcessPhotoLeptonHadron = 5,
        kProcessDecay = 6,
        kProcessGeneral = 7,
        kProcessParameterization = 8,
        kProcessUserDefined = 9
    };
    
    /// Several important process sub-types as defined by geant4.  These are
    /// copied so that reading the files does not directly depend on having
    /// geant4 installed.  Check the geant4 documentation for the
    /// documentation, but most of the names are fairly self explanatory.  The
    /// definitions are mostly found in the geant4 include files named
    /// G4HadronicProcessType.hh and G4EmProcessSubType.hh
    enum G4ProcessSubtype {
        // EM subtypes for charged particles.
        kSubtypeEMCoulombScattering = 1,
        kSubtypeEMIonization = 2,
        kSubtypeEMBremsstrahlung = 3,
        kSubtypeEMPairProdByCharged = 4,
        kSubtypeEMNuclearStopping = 8,

        // EM subtypes for photons
        kSubtypeEMMultipleScattering = 10,
        kSubtypeEMPhotoelectric = 12,
        kSubtypeEMComptonScattering = 13,
        kSubtypeEMGammaConversion = 14,
        
        // Hadronic subtypes
        kSubtypeHadronElastic = 111,
        kSubtypeHadronInelastic = 121,
        kSubtypeHadronCapture = 131,
        kSubtypeHadronChargeExchange = 161,

        // General subtypes
        kSubtypeGeneralStepLimit = 401,
    };

    /// The position of this trajectory point.
    const TLorentzVector& GetPosition() const {return Position;}

    /// The momentum of the particle at this trajectory point.
    const TVector3& GetMomentum() const {return Momentum;}

    /// The interaction process type associated with this trajectory point.
    /// The possible values are defined in the G4ProcessType enum.
    int GetProcess() const {return Process;}

    /// The interaction process type associated with this trajectory point.
    /// The possible values are defined in the G4ProcessSubtype enum.
    int GetSubprocess() const {return Subprocess;}
    
#if defined(EDEPSIM_USE_PUBLIC_FIELDS)&&!defined(EDEPSIM_FORCE_PRIVATE_FIELDS)&&!defined(__CINT__)
public:
#ifdef EDEPSIM_WARN_PUBLIC_FIELDS
#warning Using deprecated public fields.  Please consider using the accessor.  For example, to access PrimaryId, use GetPrimaryId().
#endif
#else
private:
#endif
    
    /// The position of this trajectory point.
    TLorentzVector Position;

    /// The momentum of the particle at this trajectory point.
    TVector3 Momentum;

    /// The interaction process type associated with this trajectory point.
    /// The possible values are defined in the G4ProcessType enum.
    Int_t Process;

    /// The interaction process type associated with this trajectory point.
    /// The possible values are defined in the G4ProcessSubtype enum.
    Int_t Subprocess;
    
    ClassDef(TG4TrajectoryPoint,1)
};
#endif
