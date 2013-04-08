#ifndef DSimVPositionFactory_hh_seen
#define DSimVPositionFactory_hh_seen

#include "kinem/DSimVPrimaryFactory.hh"

class DSimVPositionGenerator;

class DSimVPositionFactory : public DSimVPrimaryFactory {
public:
    DSimVPositionFactory(G4String name, 
                        DSimUserPrimaryGeneratorMessenger* fParent,
                        bool makeDirectory = true);
    virtual ~DSimVPositionFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual DSimVPositionGenerator* GetGenerator() = 0;
};
#endif
