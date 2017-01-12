#ifndef EDepSim_VTimeGenerator_hh_seen
#define EDepSim_VTimeGenerator_hh_seen

#include <G4String.hh>
#include <G4LorentzVector.hh>

/// A base class used by ND20PrimaryGenerator to select candiate vertex times.
/// Classes derived from EDepSim::VTimeGenerator are responsible for returning new
/// candidate times that are passed to EDepSim::VKinematicsGenerator by the
/// EDepSim::PrimaryGenerator object.  Each call to EDepSim::VKinematicsGenerator will
/// create a new "unique" primary vertex; however, it may ignore the candidate
/// time.  The derived classes must override two (pure virtual)
/// EDepSim::VTimeGenerator methods.  The EDepSim::VTimeGenerator::GetTime() must
/// return the time which is used to fill the time component of a
/// G4LorentzVector.  It takes the candidate vertex position generated by the
/// EDepSim::VPositionGenerator object so that it can correct for time of flight.
/// The EDepSim::VTimeGenerator::ForceTime() method must return a boolean to flag
/// whether the EDepSim::PrimaryGenerator should override the time selected by
/// EDepSim::VKinematicsGenerator with the one returned by
/// EDepSim::VTimeGenerator::GetTime().  The ForceTime method should generally
/// return true.
namespace EDepSim {class VTimeGenerator;}
class EDepSim::VTimeGenerator {
public:
    VTimeGenerator(const G4String& name) : fName(name) {}
    virtual ~VTimeGenerator() {}

    /// Return the time of the primary vertex.
    virtual double GetTime(const G4LorentzVector& vertex) = 0;

    /// Return true if this time should be forced for the primary vertex.
    virtual bool ForceTime() = 0;

    /// Return the name of the generator.
    G4String GetName() const {return fName;}

private:
    /// The name of the generator.
    G4String fName;
};
#endif
