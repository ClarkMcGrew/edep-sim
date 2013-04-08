#ifndef DSimFixedCountFactory_hh_seen
#define DSimFixedCountFactory_hh_seen

#include "kinem/DSimVCountFactory.hh"

class DSimFixedCountFactory : public DSimVCountFactory {
public:
    DSimFixedCountFactory(DSimUserPrimaryGeneratorMessenger* parent);
    virtual ~DSimFixedCountFactory();

    /// Return the fixed count generator.
    DSimVCountGenerator* GetGenerator();

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
