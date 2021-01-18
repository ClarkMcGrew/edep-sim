#ifndef EDepSim_DetectorMessenger_h
#define EDepSim_DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

namespace EDepSim {class UserDetectorConstruction;}
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

/// Handle the command line interface to change the geometry and other
/// parameters associated with the EDepSim::m detector.
namespace EDepSim {class DetectorMessenger;}
class EDepSim::DetectorMessenger: public G4UImessenger {
public:
    DetectorMessenger(EDepSim::UserDetectorConstruction*);
    virtual ~DetectorMessenger();

    void SetNewValue(G4UIcommand*, G4String);

private:
    EDepSim::UserDetectorConstruction* fConstruction;

    G4UIdirectory*             fEDepSimDir;
    G4UIdirectory*             fGDMLDir;

    G4UIcmdWithoutParameter*   fUpdateCmd;
    G4UIcmdWithAString*        fPrintMassCmd;
    G4UIcmdWithoutParameter*   fValidateCmd;
    G4UIcmdWithAString*        fExportCmd;
    G4UIcmdWithAString*        fGDMLCmd;
    G4UIcommand*               fControlCmd;
    G4UIcommand*               fHitSagittaCmd;
    G4UIcommand*               fHitLengthCmd;
    G4UIcommand*               fHitExcludedCmd;

};

#endif
