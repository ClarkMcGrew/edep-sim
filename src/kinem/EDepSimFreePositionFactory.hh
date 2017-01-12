#ifndef EDepSim_FreePositionFactory_hh_seen
#define EDepSim_FreePositionFactory_hh_seen

#include "kinem/EDepSimVPositionFactory.hh"

namespace EDepSim {class FreePositionFactory;}
class EDepSim::FreePositionFactory : public EDepSim::VPositionFactory {
public:
    FreePositionFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~FreePositionFactory();

    /// Return the fixed vertex generator.
    EDepSim::VPositionGenerator* GetGenerator();

};

#endif
