#ifndef DSimUniformPositionFactory_hh_seen
#define DSimUniformPositionFactory_hh_seen

#include "kinem/DSimVConstrainedPositionFactory.hh"

class DSimUniformPositionFactory : public DSimVConstrainedPositionFactory {
public:
    DSimUniformPositionFactory(DSimUserPrimaryGeneratorMessenger* parent);
    virtual ~DSimUniformPositionFactory();

    /// Return the uniform vertex generator.  This must call
    /// DSimVConstrainedPositionFactory::GetGenerator().
    DSimVPositionGenerator* GetGenerator();

    /// Create a new uniform vertex generator.  The newly created vertex
    /// generator will be initialized by the factory.
    DSimVPositionGenerator* CreateGenerator();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

};

#endif
