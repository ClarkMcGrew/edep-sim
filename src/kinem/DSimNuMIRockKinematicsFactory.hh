#ifndef DSimNuMIRockKinematicsFactory_hh_seen
#define DSimNuMIRockKinematicsFactory_hh_seen

#include "kinem/DSimVKinematicsFactory.hh"

class G4VPrimaryGenerator;
class DSimNuMIRockKinematicsGenerator;

class DSimNuMIRockKinematicsFactory : public DSimVKinematicsFactory {
public:
    DSimNuMIRockKinematicsFactory(DSimUserPrimaryGeneratorMessenger* fParent);
    virtual ~DSimNuMIRockKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual DSimVKinematicsGenerator* GetGenerator();

};
#endif
