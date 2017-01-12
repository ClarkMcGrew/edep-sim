#ifndef EDepSim_MeanCountFactory_hh_seen
#define EDepSim_MeanCountFactory_hh_seen

#include "kinem/EDepSimVCountFactory.hh"

namespace EDepSim {class MeanCountFactory;}
class EDepSim::MeanCountFactory : public EDepSim::VCountFactory {
public:
    MeanCountFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~MeanCountFactory();

    /// Return the mean count generator.
    EDepSim::VCountGenerator* GetGenerator();

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
