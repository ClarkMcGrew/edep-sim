#ifndef EDepSim_VConstrainedPositionFactory_hh_seen
#define EDepSim_VConstrainedPositionFactory_hh_seen

#include "kinem/EDepSimVPositionFactory.hh"

namespace EDepSim {class VConstrainedPositionGenerator;}
namespace EDepSim {class VPositionGenerator;}

namespace EDepSim {class VConstrainedPositionFactory;}
class EDepSim::VConstrainedPositionFactory : public EDepSim::VPositionFactory {
public:
    VConstrainedPositionFactory(G4String name, 
                        EDepSim::UserPrimaryGeneratorMessenger* fParent,
                        bool makeDirectory = true);
    virtual ~VConstrainedPositionFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes, but the derived generator must return the value from
    /// EDepSim::VConstrainedPositionFactory::GetGenerator().
    virtual EDepSim::VPositionGenerator* GetGenerator();

    /// Create a new generator that can be initialized.  This is a pure
    /// virtual function so it must be implemented in the derived class.
    virtual EDepSim::VPositionGenerator* CreateGenerator() = 0;

    void SetNewValue(G4UIcommand*, G4String);

private:

    EDepSim::VConstrainedPositionGenerator* fCurrent;

    G4UIcmdWithAString* fPositionSampleVolumeCMD;
    G4UIcmdWithoutParameter*   fPositionClearCMD;
    G4UIcmdWithAString* fPositionVolumeCMD;
    G4UIcmdWithAString* fPositionNotVolumeCMD;
    G4UIcmdWithAString* fPositionMaterialCMD;
    G4UIcmdWithAString* fPositionNotMaterialCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinXCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinYCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinZCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinTCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxXCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxYCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxZCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxTCMD;

};
#endif
