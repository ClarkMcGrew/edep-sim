#ifndef DSimDensityPositionFactory_hh_seen
#define DSimDensityPositionFactory_hh_seen

#include "kinem/DSimVConstrainedPositionFactory.hh"

class DSimDensityPositionFactory : public DSimVConstrainedPositionFactory {
public:
    DSimDensityPositionFactory(DSimUserPrimaryGeneratorMessenger* parent);
    virtual ~DSimDensityPositionFactory();

    /// Return the density vertex generator.  This must call
    /// DSimVConstrainedPositionFactory::GetGenerator().
    DSimVPositionGenerator* GetGenerator();

    /// Create a new density vertex generator.  The newly created vertex
    /// generator will be initialized by the factory.
    DSimVPositionGenerator* CreateGenerator();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

};

#endif
