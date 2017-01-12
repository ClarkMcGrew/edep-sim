#ifndef EDepSim_GPSKinematicsFactory_hh_seen
#define EDepSim_GPSKinematicsFactory_hh_seen

#include "kinem/EDepSimVKinematicsFactory.hh"

class G4VPrimaryGenerator;
namespace EDepSim {class GPSKinematicsGenerator;}

namespace EDepSim {class GPSKinematicsFactory;}
class EDepSim::GPSKinematicsFactory : public EDepSim::VKinematicsFactory {
public:
    GPSKinematicsFactory(EDepSim::UserPrimaryGeneratorMessenger* fParent);
    virtual ~GPSKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual EDepSim::VKinematicsGenerator* GetGenerator();

private:
    /// The GPS G4VPrimaryGenerator that will be used for this kinematics
    /// generator.  This must be created when the factory is constructed so
    /// that the GPS can add it's commands to the UI.  There can be only one.
    G4VPrimaryGenerator* fGenerator;
};
#endif
