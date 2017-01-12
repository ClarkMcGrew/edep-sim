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
/// derived from EDepSim::VKinematicsGenerator are responsible for constructing
/// an primary vertex, adding particles to the primary vertex, and adding the
/// vertex to the current G4Event.  The derived classes must override two
/// (pure virtual) methods.  The first,
/// EDepSim::VKinematicsGenerator::GeneratePrimaryVertex() does the actual work of
/// creating the new primary vertex.  The second,
/// EDepSim::VKinematicsGenerator::EventRate() is used to by EDepSim::mc to estimate
/// the number of events to be generated in a spill, and is only used by some
/// of the EDepSim::VCountGenerator derived classes.
///
/// The EDepSim::VKinematicsGenerator derived class should throw an ENoMoreEvents.
namespace EDepSim {class VKinematicsGenerator;}
class EDepSim::VKinematicsGenerator {
public:
    VKinematicsGenerator(const G4String& name) : fName(name) {}
    virtual ~VKinematicsGenerator() {}

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
namespace EDepSim {class NoMoreEvents;}
class EDepSim::NoMoreEvents : public EDepSim::Exception {
public:
    NoMoreEvents() {}
    ~NoMoreEvents() throw() {}
    const char* what(void) const throw() {return "EDepSim::NoMoreEvents";}
};

#endif

