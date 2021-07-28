#ifndef EDepSim_CreatePhysicsListMessenger_h
#define EDepSim_CreatePhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

#include "G4VModularPhysicsList.hh"
#include "EDepSimExtraPhysics.hh"

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

/// Provide control of the physics list and cut parameters
namespace EDepSim {class CreatePhysicsListMessenger;}
class EDepSim::CreatePhysicsListMessenger
    : public G4UImessenger {
public:

    CreatePhysicsListMessenger(G4VModularPhysicsList*, EDepSim::ExtraPhysics* );
    virtual ~CreatePhysicsListMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

private:

    G4VModularPhysicsList* fPhysicsList;
    EDepSim::ExtraPhysics* fExtraPhysics;

    G4UIdirectory* fDirectory;
    G4UIdirectory* fDecayDirectory;

    G4UIcmdWithADoubleAndUnit* fGammaCutCMD;
    G4UIcmdWithADoubleAndUnit* fElectCutCMD;
    G4UIcmdWithADoubleAndUnit* fPosCutCMD;
    G4UIcmdWithADoubleAndUnit* fAllCutCMD;
    G4UIcmdWithABool*          fIonizationModelCMD;


};
#endif
