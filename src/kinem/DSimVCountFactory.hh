#ifndef DSimVCountFactory_hh_seen
#define DSimVCountFactory_hh_seen

#include "kinem/DSimVPrimaryFactory.hh"

class DSimVCountGenerator;

class DSimVCountFactory : public DSimVPrimaryFactory {
public:
    DSimVCountFactory(G4String name, 
                       DSimUserPrimaryGeneratorMessenger* fParent,
                       bool makeDirectory=true);
    virtual ~DSimVCountFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual DSimVCountGenerator* GetGenerator() = 0;

    /// Set the intensity
    void SetIntensity(double v) {fIntensity = v;}
    
    /// Get the intensity.  For a beam spill MC, this will represent the
    /// protons per pulse.
    double GetIntensity() const { return fIntensity;}

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

private:
    G4UIcmdWithADouble* fIntensityCMD;

    /// The intensity for this event.  This is only documentation and is saved
    /// in the MC header.  For a beam spill MC, this will represent the
    /// protons per pulse.
    double fIntensity;
};
#endif
