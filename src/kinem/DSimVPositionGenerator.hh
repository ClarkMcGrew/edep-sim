#ifndef DSimVPositionGenerator_hh_seen
#define DSimVPositionGenerator_hh_seen

#include <G4String.hh>
#include <G4LorentzVector.hh>

/// A base class used by ND20PrimaryGenerator to select candiate vertex
/// positions.  Classes derived from DSimVPositionGenerator are responsible
/// for returning new candidate positions that are passed to
/// DSimVKinematicsGenerator by the DSimPrimaryGenerator object.  Each call
/// to DSimVKinematicsGenerator will create a new "unique" primary vertex;
/// however, it may ignore the candidate position.  The derived classes must
/// override two (pure virtual) DSimVPositionGenerator methods.  The
/// DSimVPositionGenerator::GetPosition() must return a G4LorentzVector
/// object which the xyz components filled.  The time component will be filled
/// later by a DSimVTimeGenerator derived object.  The
/// DSimVPosition::ForcePosition() method must return a boolean to flag
/// whether the DSimPrimaryGenerator should override the position selected by
/// DSimVKinematicsGenerator with the one returned by
/// DSimVPositionGenerator::GetPosition().  The ForcePosition methods should
/// generally return true.
class DSimVPositionGenerator {
public:
    DSimVPositionGenerator(const G4String& name) : fName(name) {}
    virtual ~DSimVPositionGenerator() {}

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
