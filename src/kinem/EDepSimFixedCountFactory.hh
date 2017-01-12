#ifndef EDepSim_FixedCountFactory_hh_seen
#define EDepSim_FixedCountFactory_hh_seen

#include "kinem/EDepSimVCountFactory.hh"

namespace EDepSim {class FixedCountFactory;}
class EDepSim::FixedCountFactory : public EDepSim::VCountFactory {
public:
    FixedCountFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~FixedCountFactory();

    /// Return the fixed count generator.
    EDepSim::VCountGenerator* GetGenerator();

    /// Set the count to be generated.
    void SetNumber(int count) {fNumber = count;}

    /// Get the count to be generated.
    int GetNumber() const {return fNumber;}

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

private:
    /// The count of particles to generate.
    int fNumber;

    G4UIcmdWithAnInteger* fNumberCMD;
};

#endif
