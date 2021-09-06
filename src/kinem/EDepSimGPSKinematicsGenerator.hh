#ifndef EDepSim_GPSKinematicsGenerator_hh_Seen
#define EDepSim_GPSKinematicsGenerator_hh_Seen

#include <globals.hh>
#include <G4LorentzVector.hh>

#include "kinem/EDepSimVKinematicsGenerator.hh"

class G4Event;
class G4VPrimaryGenerator;

/// This is an interface to use the General Particle Source (GPS).
namespace EDepSim {class GPSKinematicsGenerator;}
class EDepSim::GPSKinematicsGenerator : public EDepSim::VKinematicsGenerator {
public:
    GPSKinematicsGenerator(const G4String& name,
                                G4VPrimaryGenerator* gps);
    virtual ~GPSKinematicsGenerator();

    /// Add a primary vertex to the event.
    virtual GeneratorStatus
    GeneratePrimaryVertex(G4Event* evt,
                          const G4LorentzVector& position);

private:
    G4VPrimaryGenerator* fGenerator;
};
#endif
