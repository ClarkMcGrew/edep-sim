#include "kinem/EDepSimExternalKinematicsFactory.hh"

#include "EDepSimLog.hh"
#include "EDepSimException.hh"
#include "EDepSimGetExternalActionConstructor.hh"

#include <G4UIcommand.hh>
#include <G4UIcmdWithAString.hh>

EDepSim::ExternalKinematicsFactory::ExternalKinematicsFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent)
    : EDepSim::VKinematicsFactory("external",parent),
      fUserOption("unspecified"), fLibraryPath("/dev/null"),
      fSymbolName("CreateKinematicsGenerator") {

    fExternalKinemCMD = new G4UIcommand(CommandName("loadGenerator"),this);
    fExternalKinemCMD->SetGuidance("Load a kinematics generator overriding"
                                   " the library, symbol and option commands.");
    fExternalKinemCMD->SetGuidance("");
    fExternalKinemCMD->SetGuidance(
        "  ENVIRONMENT VARIABLE SUBSTITUTION: Environment variables in the");
    fExternalKinemCMD->SetGuidance(
        "  path will be expanded, but to work around the G4 macro language");
    fExternalKinemCMD->SetGuidance(
        "  they must have the syntax $ENV_VAR or $(ENV_VAR) and not use the");
    fExternalKinemCMD->SetGuidance(
        "  they must have the syntax $ENV_VAR or $(ENV_VAR) and not use the");
    fExternalKinemCMD->SetGuidance(
        "  more standard ${ENV_VAR} syntax.");

    fExternalKinemCMD->SetParameter(new G4UIparameter("library",'s',false));
    fExternalKinemCMD->GetParameter(0)
        ->SetGuidance("The path to the shared library."
                      " Environment variables will be expanded.");
    fExternalKinemCMD->SetParameter(new G4UIparameter("symbol",'s',true));
    fExternalKinemCMD->GetParameter(1)->SetDefaultValue(fSymbolName);
    fExternalKinemCMD->GetParameter(1)
        ->SetGuidance("Symbol name to call."
                      " [EDepSim::VKinemGenerator* (*)(char* option)]");
    fExternalKinemCMD->SetParameter(new G4UIparameter("option",'s',false));
    fExternalKinemCMD->GetParameter(2)
        ->SetGuidance("A user defined option for the generator.");

    fLibraryPathCMD = new G4UIcmdWithAString(CommandName("library"),this);
    fLibraryPathCMD->SetGuidance(
        "Set the shared library path with environ expansion.");
    fLibraryPathCMD->SetGuidance("");
    fLibraryPathCMD->SetGuidance(
        "  ENVIRONMENT VARIABLE SUBSTITUTION: Environment variables in the");
    fLibraryPathCMD->SetGuidance(
        "  path will be expanded, but to work around the G4 macro language");
    fLibraryPathCMD->SetGuidance(
        "  they must have the syntax $ENV_VAR or $(ENV_VAR) and not use the");
    fLibraryPathCMD->SetGuidance(
        "  they must have the syntax $ENV_VAR or $(ENV_VAR) and not use the");
    fLibraryPathCMD->SetGuidance(
        "  more standard ${ENV_VAR} syntax.");

    fLibraryPathCMD->SetParameterName("path",false);

    fSymbolNameCMD = new G4UIcmdWithAString(CommandName("symbol"),this);
    fSymbolNameCMD->SetGuidance(
        "Set the symbol name called to create the generator.");
    fSymbolNameCMD->SetGuidance(
        "    The creation routine must be defined with the signature:");
    fSymbolNameCMD->SetGuidance(
        "    [EDepSim::VKinemGenerator* ()(char* option)]");
    fSymbolNameCMD->SetParameterName("symbol-name",false);

    fUserOptionCMD = new G4UIcmdWithAString(CommandName("option"),this);
    fUserOptionCMD->SetGuidance("Set the user option for the external generator.");
    fUserOptionCMD->SetGuidance(
        "   The option is passed directly to the creation routine");
    fUserOptionCMD->SetGuidance(
        "   and can be used there for any purpose.");
    fUserOptionCMD->SetParameterName("user-option",false);

}

EDepSim::ExternalKinematicsFactory::~ExternalKinematicsFactory() {
    if (fExternalKinemCMD) delete fExternalKinemCMD;
    if (fLibraryPathCMD) delete fLibraryPathCMD;
    if (fSymbolNameCMD) delete fSymbolNameCMD;
    if (fUserOptionCMD) delete fUserOptionCMD;
}

EDepSim::VKinematicsGenerator*
EDepSim::ExternalKinematicsFactory::GetGenerator() {
    EDepSimLog("Creating an external generator");
    EDepSim::VKinematicsGenerator* generator
        = CallExternalConstructor<VKinematicsGenerator>(
            fLibraryPath, fSymbolName, fUserOption);
    if (generator == nullptr) {
            EDepSimThrow("External generator not loaded");
    }

    return generator;
}

void EDepSim::ExternalKinematicsFactory::SetNewValue(G4UIcommand* command,
                                                     G4String newValue) {
    if (command == fLibraryPathCMD) {
        SetLibraryPath(newValue);
    }
    else if (command == fSymbolNameCMD) {
        SetSymbolName(newValue);
    }
    else if (command == fUserOptionCMD) {
        SetOption(newValue);
    }
    else if (command == fExternalKinemCMD) {
        std::string library;
        std::string symbol;
        std::string option;
        std::istringstream is(newValue);
        is >> library >> symbol >> option;
        SetLibraryPath(library);
        SetSymbolName(symbol);
        SetOption(option);
    }
    else{
        EDepSimError("Nothing to set the value.");
        EDepSimThrow("Invalid New Value Error");
    }
}
