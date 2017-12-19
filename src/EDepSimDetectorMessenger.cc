// $Id: EDepSim::DetectorMessenger.cc,v 1.16 2011/03/09 18:52:49 mcgrew Exp $
//

#include "EDepSimUserDetectorConstruction.hh"
#include "EDepSimDetectorMessenger.hh"
#include "EDepSimRootGeometryManager.hh"
#include "EDepSimException.hh"

#include "globals.hh"

#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4GDMLParser.hh"
#include "EDepSimLog.hh"

#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <functional>

EDepSim::DetectorMessenger::DetectorMessenger(EDepSim::UserDetectorConstruction* d)
    : fConstruction(d) {
    fEDepSimDir = new G4UIdirectory("/edep/");
    fEDepSimDir->SetGuidance("EDepSim detector control.");

    fUpdateCmd = new G4UIcmdWithoutParameter("/edep/update",this);
    fUpdateCmd->SetGuidance("Do the run manager initialization."
                            "  This causes the geometry to be built."
                            "  This MUST be applied before \"/run/beamOn\".");
    fUpdateCmd->AvailableForStates(G4State_PreInit);

    fPrintMassCmd = new G4UIcmdWithAString("/edep/printMass",this);
    fPrintMassCmd->SetParameterName("volume-name", false);
    fPrintMassCmd->SetGuidance(
        "Print the mass of the first physical volume containing the sub-string."
        " (set before update)");
    fPrintMassCmd->AvailableForStates(G4State_PreInit);

    fValidateCmd = new G4UIcmdWithoutParameter("/edep/validateGeometry",this);
    fValidateCmd->SetGuidance(
        "Check the geometry for overlaps (set before update).");
    fValidateCmd->AvailableForStates(G4State_PreInit);

    fExportCmd = new G4UIcmdWithAString("/edep/export",this);
    fExportCmd->SetGuidance(
        "Export the geometry to a file.  This is not compatible with event\n"
        "generation, or reading of kinematics files.");
    fExportCmd->SetParameterName("RootFile", false);

    fGDMLDir = new G4UIdirectory("/edep/gdml");
    fGDMLDir->SetGuidance("Control over the gdml file.");

    fGDMLCmd = new G4UIcmdWithAString("/edep/gdml/read",this);
    fGDMLCmd->SetGuidance("Define a GDML file to be used for the geometry.");
    fGDMLCmd->AvailableForStates(G4State_PreInit);

    fMagneticFieldCmd = new G4UIcmdWithADoubleAndUnit("/edep/field",this);
    fMagneticFieldCmd->SetGuidance("Set the field strength in the UA1 magnet.");
    fMagneticFieldCmd->SetParameterName("field", false);
    fMagneticFieldCmd->SetUnitCategory("Magnetic flux density");
    fMagneticFieldCmd->AvailableForStates(G4State_PreInit);

    fControlCmd = new G4UIcommand("/edep/control",this);
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


EDepSim::DetectorMessenger::~DetectorMessenger()
{
    delete fUpdateCmd;
    delete fPrintMassCmd;
    delete fValidateCmd;
    delete fExportCmd;
    delete fGDMLCmd;
    delete fMagneticFieldCmd;
    delete fControlCmd;
    delete fEDepSimDir;
    delete fGDMLDir;
}


void EDepSim::DetectorMessenger::SetNewValue(G4UIcommand * cmd,
                                             G4String newValue) {
    if (cmd == fUpdateCmd) {
        fConstruction->UpdateGeometry();
    }
    else if (cmd == fPrintMassCmd) {
        EDepSim::RootGeometryManager::Get()->ShouldPrintMass(newValue);
    }
    else if (cmd == fValidateCmd) {
        EDepSimLog("Geometry will be validated");
        fConstruction->ValidateGeometry();
    }
    else if (cmd == fExportCmd) {
        EDepSim::RootGeometryManager::Get()->Export(newValue);
    }
    else if (cmd == fGDMLCmd) {
        EDepSimLog("Read a gdml geometry from |" << newValue << "|");
        G4GDMLParser* gdmlParser = fConstruction->GetGDMLParser();
        if (gdmlParser) delete gdmlParser;
        gdmlParser = new G4GDMLParser;
        fConstruction->SetGDMLParser(gdmlParser);
        // Read the gdml file, but don't try and validate it against the
        // schema since there's a really high chance it won't be available.
        gdmlParser->Read(newValue,false);
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
        EDepSimLog("%%%% Set Run Conditions");
        EDepSimLog("%%     Condition Name: "<< name);
        EDepSimLog("%%     Version:        " << version);
        G4UImanager* UI = G4UImanager::GetUIpointer();
        UI->ApplyCommand("/control/execute " + file);
    }

}
