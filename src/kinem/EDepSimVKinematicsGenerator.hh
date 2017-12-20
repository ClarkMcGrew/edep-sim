#ifndef EDepSim_VKinematicsGenerator_hh_Seen
#define EDepSim_VKinematicsGenerator_hh_Seen

#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>
#include <G4Material.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include "EDepSimException.hh"

class G4Event;

/// A base class for specific kinematics generators used by
/// EDepSim::PrimaryGenerator to construct G4PrimaryVertex, and the kinematics
/// for primary particles from that vertex.  Classes derived from
/// EDepSim::VKinematicsGenerator are responsible for constructing a
/// G4PrimaryVertex, adding particles to the primary vertex, and adding the
/// vertex to the current G4Event.  The derived classes must override the pure
/// virtual methods.  The GeneratePrimaryVertex() method does the actual work.
///
/// The EDepSim::VKinematicsGenerator derived class should throw an
/// ENoMoreEvents when it runs out of input events.
namespace EDepSim {class VKinematicsGenerator;}
class EDepSim::VKinematicsGenerator {
public:
    VKinematicsGenerator(const G4String& name) : fName(name) {}
    virtual ~VKinematicsGenerator() {}

    /// A status value that can be returned by GeneratePrimaryVertex.
    typedef enum {
        /// The generation failed and should be tried again.
        kFail = 0,
        /// A vertex was successfully generated.
        kSuccess,
        /// The new vertex should be generated, but it is te last one for the
        /// event.  This only has an effect if the EDepSim::PrimaryGenerator
        /// object is building a multiple vertex event.
        kLastVertex,
        /// A new vertex should not be generated and EDepSim::PrimaryGenerator
        /// should return.  It is the users responsibility to make sure that
        /// at least one primary vertex will have been generated.
        kEndEvent,
    } GeneratorStatus;

    /// Add a G4PrimaryVertex object to the event.  This should return
    /// EDepSim::VKinematicsGenerator::kFail if a vertex could not be
    /// generated for some reason (e.g. an event was discarded due to event
    /// weighting probabilities, or occurred on an illegal target).  The
    /// vertex will be constructed as normal for G4, and added using the
    /// G4Event::AddPrimaryVertex(G4PrimaryVertex*) method.
    virtual GeneratorStatus GeneratePrimaryVertex(
        G4Event* evt, const G4LorentzVector& position) = 0;
    
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

