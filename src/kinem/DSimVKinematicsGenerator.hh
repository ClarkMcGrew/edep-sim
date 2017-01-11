#ifndef DSimVKinematicsGenerator_hh_Seen
#define DSimVKinematicsGenerator_hh_Seen

#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>
#include <G4Material.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include "DSimException.hh"

class G4Event;

/// A base class used by DSimPrimaryGenerator to construct the primary
/// vertex, and generate the kinematics of the primary particles.  Classes
/// derived from DSimVKinematicsGenerator are responsible for constructing
/// an primary vertex, adding particles to the primary vertex, and adding the
/// vertex to the current G4Event.  The derived classes must override two
/// (pure virtual) methods.  The first,
/// DSimVKinematicsGenerator::GeneratePrimaryVertex() does the actual work of
/// creating the new primary vertex.  The second,
/// DSimVKinematicsGenerator::EventRate() is used to by DSimmc to estimate
/// the number of events to be generated in a spill, and is only used by some
/// of the DSimVCountGenerator derived classes.
///
/// The DSimVKinematicsGenerator derived class should throw an ENoMoreEvents.
class DSimVKinematicsGenerator {
public:
    DSimVKinematicsGenerator(const G4String& name) : fName(name) {}
    virtual ~DSimVKinematicsGenerator() {}

    /// Add a primary vertex to the event.  
    virtual bool GeneratePrimaryVertex(G4Event* evt,
                                       G4LorentzVector& position) = 0;
    
    /// Return the mean event rate per mass per 10^21 pot at a particular
    /// position and in a particular material.
    virtual double EventRate(const G4ThreeVector& pos,
                             const G4Material& material) = 0;


    /// Return the name of the generator.
    G4String GetName() const {return fName;}

private:
    /// The name of the generator.
    G4String fName;

};

/// The exception to be thrown when the generator can't make any more events.
class DSimNoMoreEvents : public DSimException {
public:
    DSimNoMoreEvents() {}
    ~DSimNoMoreEvents() throw() {}
    const char* what(void) const throw() {return "DSimNoMoreEvents";}
};

#endif

