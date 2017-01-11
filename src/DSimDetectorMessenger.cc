// $Id: DSimDetectorMessenger.cc,v 1.16 2011/03/09 18:52:49 mcgrew Exp $
//

#include "DSimUserDetectorConstruction.hh"
#include "DSimDetectorMessenger.hh"
#include "DSimRootGeometryManager.hh"
#include "DSimException.hh"

#include "globals.hh"

#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "DSimLog.hh"

#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <functional>

DSimDetectorMessenger::DSimDetectorMessenger(DSimUserDetectorConstruction* d)
    : fConstruction(d) {
    fDSimDir = new G4UIdirectory("/dsim/");
    fDSimDir->SetGuidance("T2K detector control.");

    fUpdateCmd = new G4UIcmdWithoutParameter("/dsim/update",this);
    fUpdateCmd->SetGuidance("Update geometry definition.");
    fUpdateCmd->SetGuidance("This MUST be applied before \"/run/beamOn\".");
    fUpdateCmd->AvailableForStates(G4State_PreInit);

    fValidateCmd = new G4UIcmdWithoutParameter("/dsim/validateGeometry",this);
    fValidateCmd->SetGuidance(
        "Check the geometry for overlaps (set before update).");
    fValidateCmd->AvailableForStates(G4State_PreInit);

    fExportCmd = new G4UIcmdWithAString("/dsim/export",this);
    fExportCmd->SetGuidance(
        "Export the geometry to a file.  This is not compatible with event\n"
        "generation, or reading of kinematics files.");
    fExportCmd->SetParameterName("RootFile", false);

    fMagneticFieldCmd = new G4UIcmdWithADoubleAndUnit("/dsim/field",this);
    fMagneticFieldCmd->SetGuidance("Set the field strength in the UA1 magnet.");
    fMagneticFieldCmd->SetParameterName("field", false);
    fMagneticFieldCmd->SetUnitCategory("Magnetic flux density");
    fMagneticFieldCmd->AvailableForStates(G4State_PreInit);

    fControlCmd = new G4UIcommand("/dsim/control",this);
    fControlCmd->SetGuidance(
        "Set the run conditions for this simulation.  This takes the name\n"
        "of a run condition and a version.  This is translated into a G4\n"
        "macro file which is executed.  The run condition macros are saved\n"
        "in the <package-root>/src directory (e.g. src/baseline-1.0.mac). "
        );
    G4UIparameter* par;
    // The name of the conditions
    par = new G4UIparameter('s');
    par->SetParameterName("Name");
    fControlCmd->SetParameter(par);

    // The version of the conditions
    par = new G4UIparameter('s');
    par->SetParameterName("Version");
    fControlCmd->SetParameter(par);
}


DSimDetectorMessenger::~DSimDetectorMessenger()
{
    delete fUpdateCmd;
    delete fValidateCmd;
    delete fExportCmd;
    delete fMagneticFieldCmd;
    delete fControlCmd;
    delete fDSimDir;
}


void DSimDetectorMessenger::SetNewValue(G4UIcommand * cmd,
                                         G4String newValue) {
    if (cmd == fUpdateCmd) {
        fConstruction->UpdateGeometry();
    }
    else if (cmd == fValidateCmd) {
        DSimLog("Geometry will be validated");
        fConstruction->ValidateGeometry();
    }
    else if (cmd == fExportCmd) {
        DSimRootGeometryManager::Get()->Export(newValue);
    }
    else if (cmd == fControlCmd) {
        std::istringstream input((const char*)newValue);
        std::string name;
        std::string version;
        input >> name >> version;
#define STRINGIFY(s) #s
#define STRINGIFY_DEFINITION(s) STRINGIFY(s)
        std::string packageroot(STRINGIFY_DEFINITION(DETSIM_INSTALL_PREFIX));
        std::string::iterator new_end
            = std::remove(packageroot.begin(), packageroot.end(), '"');
        packageroot.erase(new_end, packageroot.end());
        std::string file = packageroot + "/lib/edep-sim/"
            + name + "-" + version + ".mac";
        DSimLog("%%%% Set Run Conditions");
        DSimLog("%%     Condition Name: "<< name);
        DSimLog("%%     Version:        " << version);
        G4UImanager* UI = G4UImanager::GetUIpointer();
        UI->ApplyCommand("/control/execute " + file);
    }

}
