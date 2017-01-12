#ifndef EDepSim_FixedTimeFactory_hh_seen
#define EDepSim_FixedTimeFactory_hh_seen

#include "kinem/EDepSimVTimeFactory.hh"

namespace EDepSim {class FixedTimeFactory;}
class EDepSim::FixedTimeFactory : public EDepSim::VTimeFactory {
public:
    FixedTimeFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~FixedTimeFactory();

    /// Return the fixed time generator.
    EDepSim::VTimeGenerator* GetGenerator();

    /// Set the time to be generated.
    void SetTime(double time) {fTime = time;}

    /// Get the time to be generated.
    double GetTime() const {return fTime;}

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

private:
    /// The time of particles to generate.
    double fTime;

    G4UIcmdWithADoubleAndUnit* fTimeCMD;
};

#endif
