// $Id: EDepSim::DetectorMessenger.cc,v 1.16 2011/03/09 18:52:49 mcgrew Exp $
//

#include "EDepSimUserDetectorConstruction.hh"
#include "EDepSimDetectorMessenger.hh"
#include "EDepSimRootGeometryManager.hh"
#include "EDepSimException.hh"
#include "EDepSimSDFactory.hh"
#include "EDepSimSegmentSD.hh"

#include "EDepSimLog.hh"

#include "globals.hh"

#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4GDMLParser.hh"
#include "G4UnitsTable.hh"

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

    fGDMLDir = new G4UIdirectory("/edep/gdml/");
    fGDMLDir->SetGuidance("Control over the gdml file.");

    fGDMLCmd = new G4UIcmdWithAString("/edep/gdml/read",this);
    fGDMLCmd->SetGuidance("Define a GDML file to be used for the geometry.");
    fGDMLCmd->AvailableForStates(G4State_PreInit);

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

    fHitSagittaCmd = new G4UIcommand("/edep/hitSagitta",this);
    fHitSagittaCmd->SetGuidance(
        "Set the maximum sagitta for hit segments.");
    fHitSagittaCmd->AvailableForStates(G4State_PreInit);

    // The name of the sensitive detector.
    par = new G4UIparameter("Sensitive", 's', false);
    fHitSagittaCmd->SetParameter(par);

    // The name of the sensitive detector.
    par = new G4UIparameter("Sagitta", 'd', false);
    fHitSagittaCmd->SetParameter(par);
    
    // The name of the sensitive detector.
    par = new G4UIparameter("Unit", 's', false);
    fHitSagittaCmd->SetParameter(par);

    fHitLengthCmd = new G4UIcommand("/edep/hitLength",this);
    fHitLengthCmd->SetGuidance(
        "Set the maximum sagitta for hit segments.");
    fHitLengthCmd->AvailableForStates(G4State_PreInit);

    // The name of the sensitive detector.
    par = new G4UIparameter("Sensitive", 's', false);
    fHitLengthCmd->SetParameter(par);

    // The name of the sensitive detector.
    par = new G4UIparameter("Length", 'd', false);
    fHitLengthCmd->SetParameter(par);
    
    // The name of the sensitive detector.
    par = new G4UIparameter("Unit", 's', false);
    fHitLengthCmd->SetParameter(par);

}


EDepSim::DetectorMessenger::~DetectorMessenger()
{
    delete fUpdateCmd;
    delete fPrintMassCmd;
    delete fValidateCmd;
    delete fExportCmd;
    delete fGDMLCmd;
    delete fControlCmd;
    delete fHitSagittaCmd;
    delete fHitLengthCmd;
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
    else if (cmd == fHitSagittaCmd) {
        std::istringstream input((const char*)newValue);
        std::string sdName;
        double sagitta;
        std::string unitName;
        input >> sdName >> sagitta >> unitName;
        sagitta *= G4UnitDefinition::GetValueOf(unitName);
        SDFactory factory("segment");
        SegmentSD* sd = dynamic_cast<SegmentSD*>(factory.MakeSD(sdName));
        if (sd) {
            sd->SetMaximumHitSagitta(sagitta);
        }
        else {
            std::cout << "Invalid sensitive detector" << std::endl;
        }
    }
    else if (cmd == fHitLengthCmd) {
        std::istringstream input((const char*)newValue);
        std::string sdName;
        double length;
        std::string unitName;
        input >> sdName >> length >> unitName;
        length *= G4UnitDefinition::GetValueOf(unitName);
        SDFactory factory("segment");
        SegmentSD* sd = dynamic_cast<SegmentSD*>(factory.MakeSD(sdName));
        if (sd) {
            sd->SetMaximumHitLength(length);
        }
        else {
            std::cout << "Invalid sensitive detector" << std::endl;
        }
    }

}
