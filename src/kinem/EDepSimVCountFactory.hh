#ifndef EDepSim_VCountFactory_hh_seen
#define EDepSim_VCountFactory_hh_seen

#include "kinem/EDepSimVPrimaryFactory.hh"

namespace EDepSim {class VCountGenerator;}

namespace EDepSim {class VCountFactory;}
class EDepSim::VCountFactory : public EDepSim::VPrimaryFactory {
public:
    VCountFactory(G4String name, 
                       EDepSim::UserPrimaryGeneratorMessenger* fParent,
                       bool makeDirectory=true);
    virtual ~VCountFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual EDepSim::VCountGenerator* GetGenerator() = 0;

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
