#ifndef DSimFixedPositionFactory_hh_seen
#define DSimFixedPositionFactory_hh_seen

#include "kinem/DSimVPositionFactory.hh"

class DSimFixedPositionFactory : public DSimVPositionFactory {
public:
    DSimFixedPositionFactory(DSimUserPrimaryGeneratorMessenger* parent);
    virtual ~DSimFixedPositionFactory();

    /// Return the fixed vertex generator.
    DSimVPositionGenerator* GetGenerator();

    /// Return the position for the next generator.
    G4ThreeVector GetPosition() {return fPosition;}

    /// Set the position for the next generator.
    void SetPosition(const G4ThreeVector& pos) {fPosition = pos;}

    void SetNewValue(G4UIcommand*, G4String);

private:
    G4ThreeVector fPosition;

    G4UIcmdWith3VectorAndUnit* fPositionCMD;
};
#endif
