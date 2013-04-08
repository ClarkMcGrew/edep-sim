#ifndef DSimDetectorMessenger_h
#define DSimDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DSimUserDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

/// Handle the command line interface to change the geometry and other
/// parameters associated with the DSimm detector.  
class DSimDetectorMessenger: public G4UImessenger {
public:
    DSimDetectorMessenger(DSimUserDetectorConstruction*);
    virtual ~DSimDetectorMessenger();

    void SetNewValue(G4UIcommand*, G4String);

private:
    DSimUserDetectorConstruction* fConstruction;

    G4UIdirectory*             fDSimDir;

    G4UIcmdWithoutParameter*   fUpdateCmd;
    G4UIcmdWithoutParameter*   fValidateCmd;
    G4UIcmdWithAString*        fExportCmd;
    G4UIcommand*               fControlCmd;
    G4UIcmdWithADoubleAndUnit* fMagneticFieldCmd;

};

#endif
