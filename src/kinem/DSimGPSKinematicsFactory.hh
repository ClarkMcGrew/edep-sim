#ifndef DSimGPSKinematicsFactory_hh_seen
#define DSimGPSKinematicsFactory_hh_seen

#include "kinem/DSimVKinematicsFactory.hh"

class G4VPrimaryGenerator;
class DSimGPSKinematicsGenerator;

class DSimGPSKinematicsFactory : public DSimVKinematicsFactory {
public:
    DSimGPSKinematicsFactory(DSimUserPrimaryGeneratorMessenger* fParent);
    virtual ~DSimGPSKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual DSimVKinematicsGenerator* GetGenerator();

private:
    /// The GPS G4VPrimaryGenerator that will be used for this kinematics
    /// generator.  This must be created when the factory is constructed so
    /// that the GPS can add it's commands to the UI.  There can be only one.
    G4VPrimaryGenerator* fGenerator;
};
#endif
