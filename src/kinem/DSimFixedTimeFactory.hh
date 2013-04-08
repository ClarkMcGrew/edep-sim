#ifndef DSimFixedTimeFactory_hh_seen
#define DSimFixedTimeFactory_hh_seen

#include "kinem/DSimVTimeFactory.hh"

class DSimFixedTimeFactory : public DSimVTimeFactory {
public:
    DSimFixedTimeFactory(DSimUserPrimaryGeneratorMessenger* parent);
    virtual ~DSimFixedTimeFactory();

    /// Return the fixed time generator.
    DSimVTimeGenerator* GetGenerator();

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
