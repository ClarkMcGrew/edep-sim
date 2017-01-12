#ifndef EDepSim_VTimeFactory_hh_seen
#define EDepSim_VTimeFactory_hh_seen

#include "kinem/EDepSimVPrimaryFactory.hh"

namespace EDepSim {class VTimeGenerator;}

namespace EDepSim {class VTimeFactory;}
class EDepSim::VTimeFactory : public EDepSim::VPrimaryFactory {
public:
    VTimeFactory(G4String name, 
                       EDepSim::UserPrimaryGeneratorMessenger* fParent,
                       bool makeDirectory=true);
    virtual ~VTimeFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual EDepSim::VTimeGenerator* GetGenerator() = 0;
};
#endif
