#ifndef DSimVCountGenerator_hh_seen
#define DSimVCountGenerator_hh_seen

#include <G4String.hh>

/// A base class used by ND20PrimaryGenerator to select the number of primary
/// vertices to be generated for a G4Event.  Classes derived from
/// DSimVCountGenerator are responsible for returning the number of
/// successful calls to be made to the DSimVKinematicsGenerator by the
/// DSimPrimaryGenerator object.  Each call to DSimVKinematicsGenerator will
/// create a new "unique" primary vertex; however, it may result in multiple
/// G4PrimaryVertex objects being added to the G4Event.  The derived classes
/// must override the (pure virtual) DSimVCountGenerator::GetCount() object.
/// This method must return an integer for the number of verticies to be
/// generated.  This count can be zero (or negative), in which case no
/// verticies will be generated.
class DSimVCountGenerator {
public:
    DSimVCountGenerator(const G4String& name, double intensity) 
        : fName(name), fIntensity(intensity) {}
    virtual ~DSimVCountGenerator() {}

    /// Return the number of events that should be generated.  If the return
    /// value is less than or equal to zero, then no events are generated.
    virtual int GetCount() = 0;

    /// Return the name of the generator.
    G4String GetName() const {return fName;}

    /// Return the intensity for this generator.  For a beam MC, this will be
    /// the number of protons per pulse.
    double GetIntensity() const {return fIntensity;}

private:
    /// The name of the generator.
    G4String fName;

    /// The intensity for the generator.
    double fIntensity;

};
#endif
