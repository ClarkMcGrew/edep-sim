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
        "Set the length for hit segments to stop growing.");
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

    fHitExcludedCmd = new G4UIcommand("/edep/hitExcluded",this);
    fHitExcludedCmd->SetGuidance(
        "Exclude logical volumes from being sensitive.");
    fHitExcludedCmd->AvailableForStates(G4State_PreInit);

    // The name of the sensitive detector volume
    par = new G4UIparameter("LogicalVolume", 's', false);
    fHitExcludedCmd->SetParameter(par);

    /////////////////////////////////////////////////////////////
    // Add commands for the /edep/gdml/ sub-directory
    fGDMLDir = new G4UIdirectory("/edep/gdml/");
    fGDMLDir->SetGuidance("Control over the gdml file.");

    fGDMLReadCmd = new G4UIcmdWithAString("/edep/gdml/read",this);
    fGDMLReadCmd->SetGuidance("Set a GDML file to be used for the geometry.");
    fGDMLReadCmd->AvailableForStates(G4State_PreInit);

    /////////////////////////////////////////////////////////////
    // Add commands for the /edep/material/ sub-directory
    //
    // These provide ways to inject information into the materials when it's
    // not supported by the geometry creation tools.
    fMaterialDir = new G4UIdirectory("/edep/material/");
    fMaterialDir->SetGuidance("Extra control over the material definitions.");

    fMaterialBirksCmd = new G4UIcommand("/edep/material/birksConstant",this);
    fMaterialBirksCmd->SetGuidance("Override the Birks constant for material.");
    fMaterialBirksCmd->AvailableForStates(G4State_PreInit);

    // The name of the material get a Birks constant.
    par = new G4UIparameter("Material", 's', false);
    fMaterialBirksCmd->SetParameter(par);

    // The value of the Birks constant.
    par = new G4UIparameter("Birks", 'd', false);
    fMaterialBirksCmd->SetParameter(par);

    // The unit for the Birks constant.
    par = new G4UIparameter("Unit", 's', false);
    fMaterialBirksCmd->SetParameter(par);

}


EDepSim::DetectorMessenger::~DetectorMessenger()
{
    delete fUpdateCmd;
    delete fPrintMassCmd;
    delete fValidateCmd;
    delete fExportCmd;
    delete fControlCmd;
    delete fHitSagittaCmd;
    delete fHitLengthCmd;
    delete fHitExcludedCmd;
    delete fGDMLReadCmd;
    delete fGDMLDir;
    delete fMaterialBirksCmd;
    delete fMaterialDir;
    delete fEDepSimDir;
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
    else if (cmd == fGDMLReadCmd) {
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
        std::string packageroot;
        const char *rootEnv = std::getenv("EDEPSIM_ROOT");
        if (rootEnv) {
            packageroot=rootEnv;
        }
        else {
#define STRINGIFY(s) #s
#define STRINGIFY_DEFINITION(s) STRINGIFY(s)
            packageroot = STRINGIFY_DEFINITION(DETSIM_INSTALL_PREFIX);
        }
        std::string::iterator new_end
            = std::remove(packageroot.begin(), packageroot.end(), '"');
        packageroot.erase(new_end, packageroot.end());
        std::string file = packageroot + "/lib/EDepSim/"
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
    else if (cmd == fHitExcludedCmd) {
        std::istringstream input((const char*)newValue);
        std::string logName;
        input >> logName;
        fConstruction->AddExcludedSensitiveDetector(logName);
    }
    else if (cmd == fMaterialBirksCmd) {
        std::istringstream input((const char*)newValue);
        std::string matName;
        double birksConstant;
        std::string unitName;
        input >> matName >> birksConstant >> unitName;

        // Get the material that needs to be updated.
        G4Material *material = G4Material::GetMaterial(matName.c_str());
        if (!material) {
            EDepSimError("Unknown material for /edep/material/SetBirksConstant:"
                         << " " << matName);
            throw std::runtime_error("Unknown material");
        }

        // parse the unitName which should be [length]/[energy]
        std::size_t divide = unitName.find("/");
        if (divide == std::string::npos) {
            EDepSimError("Illegal unit name for"
                         << " /edep/material/SetBirksConstant: "
                         << unitName);
            throw std::runtime_error("Illegal input");
        }
        std::string numerator = unitName.substr(0,divide);
        std::string denominator = unitName.substr(divide+1);
        birksConstant *= G4UnitDefinition::GetValueOf(numerator);
        birksConstant /= G4UnitDefinition::GetValueOf(denominator);

        material->GetIonisation()->SetBirksConstant(birksConstant);

        EDepSimLog("Set Birks constant for"
                   << " " << matName
                   << " to " << birksConstant/(mm/MeV) << " mm/MeV");
    }
}
