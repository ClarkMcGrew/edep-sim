#ifndef EDepSim_NuMIRockKinematicsFactory_hh_seen
#define EDepSim_NuMIRockKinematicsFactory_hh_seen

#include "kinem/EDepSimVKinematicsFactory.hh"

class G4VPrimaryGenerator;
namespace EDepSim {class NuMIRockKinematicsGenerator;}

namespace EDepSim {class NuMIRockKinematicsFactory;}
class EDepSim::NuMIRockKinematicsFactory : public EDepSim::VKinematicsFactory {
public:
    NuMIRockKinematicsFactory(EDepSim::UserPrimaryGeneratorMessenger* fParent);
    virtual ~NuMIRockKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual EDepSim::VKinematicsGenerator* GetGenerator();

};
#endif
