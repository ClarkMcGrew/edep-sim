#ifndef DSimPhysicsListMessenger_h
#define DSimPhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DSimPhysicsList;

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

/// Provide control of the physics list and cut parameters
class DSimPhysicsListMessenger 
    : public G4UImessenger {
public:
    
    DSimPhysicsListMessenger(DSimPhysicsList* );
    virtual ~DSimPhysicsListMessenger();
 
    virtual void SetNewValue(G4UIcommand*, G4String);

private:

    DSimPhysicsList* fPhysicsList;
 
    G4UIdirectory* fDirectory;
    G4UIdirectory* fDecayDirectory;

    G4UIcmdWithADoubleAndUnit* fGammaCutCMD;
    G4UIcmdWithADoubleAndUnit* fElectCutCMD;
    G4UIcmdWithADoubleAndUnit* fPosCutCMD;
    G4UIcmdWithADoubleAndUnit* fAllCutCMD;

};
#endif
