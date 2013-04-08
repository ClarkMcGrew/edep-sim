#ifndef DSimMeanCountFactory_hh_seen
#define DSimMeanCountFactory_hh_seen

#include "kinem/DSimVCountFactory.hh"

class DSimMeanCountFactory : public DSimVCountFactory {
public:
    DSimMeanCountFactory(DSimUserPrimaryGeneratorMessenger* parent);
    virtual ~DSimMeanCountFactory();

    /// Return the mean count generator.
    DSimVCountGenerator* GetGenerator();

    /// Set the count to be generated.
    void SetNumber(double mean) {fNumber = mean;}

    /// Get the count to be generated.
    double GetNumber() const {return fNumber;}

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

private:
    /// The count of particles to generate.
    double fNumber;

    G4UIcmdWithADouble* fNumberCMD;
};

#endif
