#ifndef EDepSim_UniformPositionFactory_hh_seen
#define EDepSim_UniformPositionFactory_hh_seen

#include "kinem/EDepSimVConstrainedPositionFactory.hh"

namespace EDepSim {class UniformPositionFactory;}
class EDepSim::UniformPositionFactory : public EDepSim::VConstrainedPositionFactory {
public:
    UniformPositionFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~UniformPositionFactory();

    /// Return the uniform vertex generator.  This must call
    /// EDepSim::VConstrainedPositionFactory::GetGenerator().
    EDepSim::VPositionGenerator* GetGenerator();

    /// Create a new uniform vertex generator.  The newly created vertex
    /// generator will be initialized by the factory.
    EDepSim::VPositionGenerator* CreateGenerator();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

};

#endif
