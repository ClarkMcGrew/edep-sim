#ifndef EDepSim_VKinematicsFactory_hh_seen
#define EDepSim_VKinematicsFactory_hh_seen

#include "kinem/EDepSimVPrimaryFactory.hh"

namespace EDepSim {class VKinematicsGenerator;}

namespace EDepSim {class VKinematicsFactory;}
class EDepSim::VKinematicsFactory : public EDepSim::VPrimaryFactory {
public:
    VKinematicsFactory(G4String name, 
                            EDepSim::UserPrimaryGeneratorMessenger* fParent,
                            bool makeDirectory=true);
    virtual ~VKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual EDepSim::VKinematicsGenerator* GetGenerator() = 0;
};
#endif
