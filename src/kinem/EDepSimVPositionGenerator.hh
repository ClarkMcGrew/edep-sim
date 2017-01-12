#ifndef EDepSim_VPositionGenerator_hh_seen
#define EDepSim_VPositionGenerator_hh_seen

#include <G4String.hh>
#include <G4LorentzVector.hh>

/// A base class used by ND20PrimaryGenerator to select candiate vertex
/// positions.  Classes derived from EDepSim::VPositionGenerator are responsible
/// for returning new candidate positions that are passed to
/// EDepSim::VKinematicsGenerator by the EDepSim::PrimaryGenerator object.  Each call
/// to EDepSim::VKinematicsGenerator will create a new "unique" primary vertex;
/// however, it may ignore the candidate position.  The derived classes must
/// override two (pure virtual) EDepSim::VPositionGenerator methods.  The
/// EDepSim::VPositionGenerator::GetPosition() must return a G4LorentzVector
/// object which the xyz components filled.  The time component will be filled
/// later by a EDepSim::VTimeGenerator derived object.  The
/// EDepSim::VPosition::ForcePosition() method must return a boolean to flag
/// whether the EDepSim::PrimaryGenerator should override the position selected by
/// EDepSim::VKinematicsGenerator with the one returned by
/// EDepSim::VPositionGenerator::GetPosition().  The ForcePosition methods should
/// generally return true.
namespace EDepSim {class VPositionGenerator;}
class EDepSim::VPositionGenerator {
public:
    VPositionGenerator(const G4String& name) : fName(name) {}
    virtual ~VPositionGenerator() {}

    /// Return the candidate position for the primary vertex.
    virtual G4LorentzVector GetPosition() = 0;

    /// Return true if this position should be forced for the primary vertex
    /// (should generally return true).
    virtual bool ForcePosition() = 0;

    /// Return the name of the generator.
    G4String GetName() const {return fName;}

private:
    /// The name of the generator.
    G4String fName;
    
};
#endif
