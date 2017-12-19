#ifndef EDepSim_NuMIRockKinematicsGenerator_hh
#define EDepSim_NuMIRockKinematicsGenerator_hh
////////////////////////////////////////////////////////////
//

#include "kinem/EDepSimVKinematicsGenerator.hh"

class G4Event;

/// This is an interface approximate the rock muons entering CAPTAIN.
namespace EDepSim {class NuMIRockKinematicsGenerator;}
class EDepSim::NuMIRockKinematicsGenerator : public EDepSim::VKinematicsGenerator {
public:
    ///  Construct a new generator.  The name is the name of the generator
    ///  (e.g. GENIE, rock, &c).  The fluxName is the name of the neutrino
    ///  spectrum.
    NuMIRockKinematicsGenerator(const G4String& name, 
                                    const G4String& fluxName);
    virtual ~NuMIRockKinematicsGenerator();

    /// Add a primary vertex to the event.  
    virtual bool GeneratePrimaryVertex(G4Event* evt,
                                       G4LorentzVector& position);

private:
    /// The static part of the file name field.
    std::string fFluxName;

};
#endif
