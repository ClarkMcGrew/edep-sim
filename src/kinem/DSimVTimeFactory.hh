#ifndef DSimVTimeFactory_hh_seen
#define DSimVTimeFactory_hh_seen

#include "kinem/DSimVPrimaryFactory.hh"

class DSimVTimeGenerator;

class DSimVTimeFactory : public DSimVPrimaryFactory {
public:
    DSimVTimeFactory(G4String name, 
                       DSimUserPrimaryGeneratorMessenger* fParent,
                       bool makeDirectory=true);
    virtual ~DSimVTimeFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual DSimVTimeGenerator* GetGenerator() = 0;
};
#endif
