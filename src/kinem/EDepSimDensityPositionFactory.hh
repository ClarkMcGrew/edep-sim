#ifndef EDepSim_DensityPositionFactory_hh_seen
#define EDepSim_DensityPositionFactory_hh_seen

#include "kinem/EDepSimVConstrainedPositionFactory.hh"

namespace EDepSim {class DensityPositionFactory;}
class EDepSim::DensityPositionFactory : public EDepSim::VConstrainedPositionFactory {
public:
    DensityPositionFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~DensityPositionFactory();

    /// Return the density vertex generator.  This must call
    /// EDepSim::VConstrainedPositionFactory::GetGenerator().
    EDepSim::VPositionGenerator* GetGenerator();

    /// Create a new density vertex generator.  The newly created vertex
    /// generator will be initialized by the factory.
    EDepSim::VPositionGenerator* CreateGenerator();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

};

#endif
