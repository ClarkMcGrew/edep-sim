#ifndef EDepSim_VKinematicsGenerator_hh_Seen
#define EDepSim_VKinematicsGenerator_hh_Seen

#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>
#include <G4Material.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include "EDepSimException.hh"

class G4Event;

/// A base class used by EDepSim::PrimaryGenerator to construct the primary
/// vertex, and generate the kinematics of the primary particles.  Classes
/// derived from EDepSim::VKinematicsGenerator are responsible for
/// constructing an primary vertex, adding particles to the primary vertex,
/// and adding the vertex to the current G4Event.  The derived classes must
/// override the pure virtual methods.  The first,
/// EDepSim::VKinematicsGenerator::GeneratePrimaryVertex() does the actual
/// work of creating the new primary vertex. 
///
/// The EDepSim::VKinematicsGenerator derived class should throw an
/// ENoMoreEvents when it runs out of input events.
namespace EDepSim {class VKinematicsGenerator;}
class EDepSim::VKinematicsGenerator {
public:
    VKinematicsGenerator(const G4String& name) : fName(name) {}
    virtual ~VKinematicsGenerator() {}

    /// Add a primary vertex to the event.  This should return false if a
    /// vertex could not be generated for some reason (e.g. an event was
    /// discarded due to event weighting probabilities, or occurred on an
    /// illegal target).
    virtual bool GeneratePrimaryVertex(G4Event* evt,
                                       G4LorentzVector& position) = 0;
    
    /// Return the name of the generator.
    G4String GetName() const {return fName;}

private:
    /// The name of the generator.
    G4String fName;

};

/// The exception to be thrown when the generator can't make any more events.
namespace EDepSim {class NoMoreEvents;}
class EDepSim::NoMoreEvents : public EDepSim::Exception {
public:
    NoMoreEvents() {}
    ~NoMoreEvents() throw() {}
    const char* what(void) const throw() {return "EDepSim::NoMoreEvents";}
};

#endif

