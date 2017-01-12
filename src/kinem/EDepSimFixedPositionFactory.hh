#ifndef EDepSim_FixedPositionFactory_hh_seen
#define EDepSim_FixedPositionFactory_hh_seen

#include "kinem/EDepSimVPositionFactory.hh"

namespace EDepSim {class FixedPositionFactory;}
class EDepSim::FixedPositionFactory : public EDepSim::VPositionFactory {
public:
    FixedPositionFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~FixedPositionFactory();

    /// Return the fixed vertex generator.
    EDepSim::VPositionGenerator* GetGenerator();

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
