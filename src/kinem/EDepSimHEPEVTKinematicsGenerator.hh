#ifndef EDepSim_HEPEVTKinematicsGenerator_hh_Seen
#define EDepSim_HEPEVTKinematicsGenerator_hh_Seen

#include <globals.hh>
#include <G4LorentzVector.hh>

#include "kinem/EDepSimVKinematicsGenerator.hh"

class G4Event;
class G4VPrimaryGenerator;

/// A base class for primary generator classes used in the EDepSim::.  This is
/// used by the EDepSim::UserPrimaryGeneratorAction to generate particles which
/// will be tracked by the G4 simulation.
namespace EDepSim {class HEPEVTKinematicsGenerator;}
class EDepSim::HEPEVTKinematicsGenerator :
    public EDepSim::VKinematicsGenerator {
public:
    HEPEVTKinematicsGenerator(const G4String& name,
                              const G4String& fileName,
                              int verbosity = 0);
    virtual ~HEPEVTKinematicsGenerator();

    /// Add a primary vertex to the event.
    virtual GeneratorStatus
    GeneratePrimaryVertex(G4Event* evt,
                          const G4LorentzVector& position);

private:
    G4VPrimaryGenerator* fGenerator;
};
#endif
