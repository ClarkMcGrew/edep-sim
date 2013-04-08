#ifndef DSimVKinematicsFactory_hh_seen
#define DSimVKinematicsFactory_hh_seen

#include "kinem/DSimVPrimaryFactory.hh"

class DSimVKinematicsGenerator;

class DSimVKinematicsFactory : public DSimVPrimaryFactory {
public:
    DSimVKinematicsFactory(G4String name, 
                            DSimUserPrimaryGeneratorMessenger* fParent,
                            bool makeDirectory=true);
    virtual ~DSimVKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual DSimVKinematicsGenerator* GetGenerator() = 0;
};
#endif
