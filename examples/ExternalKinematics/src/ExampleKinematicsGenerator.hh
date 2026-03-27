#ifndef ExampleKinematicsGenerator_hh_Seen
#define ExampleKinematicsGenerator_hh_Seen

#include <globals.hh>
#include <G4LorentzVector.hh>

#include "kinem/EDepSimVKinematicsGenerator.hh"

class G4Event;
class G4VPrimaryGenerator;

/// This is an interface to use the General Particle Source (GPS).
class ExampleKinematicsGenerator : public EDepSim::VKinematicsGenerator {
public:
    ExampleKinematicsGenerator(const G4String& name);
    virtual ~ExampleKinematicsGenerator();

    /// Add a primary vertex to the event.
    virtual GeneratorStatus
    GeneratePrimaryVertex(G4Event* evt,
                          const G4LorentzVector& position);

};
#endif
