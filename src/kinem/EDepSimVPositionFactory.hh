#ifndef EDepSim_VPositionFactory_hh_seen
#define EDepSim_VPositionFactory_hh_seen

#include "kinem/EDepSimVPrimaryFactory.hh"

namespace EDepSim {class VPositionGenerator;}

namespace EDepSim {class VPositionFactory;}
class EDepSim::VPositionFactory : public EDepSim::VPrimaryFactory {
public:
    VPositionFactory(G4String name, 
                        EDepSim::UserPrimaryGeneratorMessenger* fParent,
                        bool makeDirectory = true);
    virtual ~VPositionFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual EDepSim::VPositionGenerator* GetGenerator() = 0;
};
#endif
