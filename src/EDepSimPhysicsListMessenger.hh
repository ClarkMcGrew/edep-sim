#ifndef EDepSim_PhysicsListMessenger_h
#define EDepSim_PhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

namespace EDepSim {class PhysicsList;}

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

/// Provide control of the physics list and cut parameters
namespace EDepSim {class PhysicsListMessenger;}
class EDepSim::PhysicsListMessenger
    : public G4UImessenger {
public:
    
    PhysicsListMessenger(EDepSim::PhysicsList* );
    virtual ~PhysicsListMessenger();
 
    virtual void SetNewValue(G4UIcommand*, G4String);

private:

    EDepSim::PhysicsList* fPhysicsList;
 
    G4UIdirectory* fDirectory;
    G4UIdirectory* fDecayDirectory;

    G4UIcmdWithADoubleAndUnit* fGammaCutCMD;
    G4UIcmdWithADoubleAndUnit* fElectCutCMD;
    G4UIcmdWithADoubleAndUnit* fPosCutCMD;
    G4UIcmdWithADoubleAndUnit* fAllCutCMD;
    G4UIcmdWithABool*          fIonizationModelCMD;

};
#endif
