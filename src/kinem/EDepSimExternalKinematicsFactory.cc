#include "kinem/EDepSimExternalKinematicsFactory.hh"

#include "EDepSimLog.hh"
#include "EDepSimException.hh"
#include "EDepSimGetExternalActionConstructor.hh"

#include <G4UIcommand.hh>
#include <G4UIcmdWithAString.hh>

EDepSim::ExternalKinematicsFactory::ExternalKinematicsFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent)
    : EDepSim::VKinematicsFactory("external",parent),
      fUserName("unspecified"), fLibraryPath("/dev/null"),
      fSymbolName("CreateKinematicsGenerator") {

    fExternalKinemCMD = new G4UIcommand(CommandName("kinemConstructor"),this);
    fExternalKinemCMD->SetGuidance("Declare shared library with the external"
                                   " kinematics generator.");
    fExternalKinemCMD->SetParameter(new G4UIparameter("path",'s',false));
    fExternalKinemCMD->GetParameter(0)
        ->SetGuidance("The path to the shared library."
                      " Environment variables will be expanded.");
    fExternalKinemCMD->SetParameter(new G4UIparameter("symbol",'s',true));
    fExternalKinemCMD->GetParameter(1)->SetDefaultValue(fSymbolName);
    fExternalKinemCMD->GetParameter(1)
        ->SetGuidance("Symbol name to call."
                      " [signature EDepSim::VKinemGenerator* (*)(char* name)]");
    fExternalKinemCMD->SetParameter(new G4UIparameter("name",'s',false));
    fExternalKinemCMD->GetParameter(2)
        ->SetGuidance("A user defined name for the generator.");

    fUserNameCMD = new G4UIcmdWithAString(CommandName("name"),this);
    fUserNameCMD->SetGuidance("Set the user name for the external generator.");
    fUserNameCMD->SetGuidance(
        "   The name is passed directly to the creation routine");
    fUserNameCMD->SetGuidance(
        "   and can be used there for any purpose.");
    fUserNameCMD->SetParameterName("user-name",false);

    fLibraryPathCMD = new G4UIcmdWithAString(CommandName("path"),this);
    fLibraryPathCMD->SetGuidance(
        "Set the shared library path with environ expansion.");
    fLibraryPathCMD->SetParameterName("path",false);

    fSymbolNameCMD = new G4UIcmdWithAString(CommandName("symbol"),this);
    fSymbolNameCMD->SetGuidance(
        "Set the symbol name called to create the generator.");
    fSymbolNameCMD->SetGuidance(
        "    The creation routine must be defined with the signature:");
    fSymbolNameCMD->SetGuidance(
        "    [signature EDepSim::VKinemGenerator* ()(char* name)]");
    fSymbolNameCMD->SetParameterName("symbol-name",false);
}

EDepSim::ExternalKinematicsFactory::~ExternalKinematicsFactory() {
    if (fExternalKinemCMD) delete fExternalKinemCMD;
    if (fUserNameCMD) delete fUserNameCMD;
    if (fLibraryPathCMD) delete fLibraryPathCMD;
    if (fSymbolNameCMD) delete fSymbolNameCMD;
}

EDepSim::VKinematicsGenerator*
EDepSim::ExternalKinematicsFactory::GetGenerator() {
    EDepSimLog("Creating an external generator");
    EDepSim::LogManager::IncreaseIndentation();
    EDepSimLog("User name:      " << fUserName);
    EDepSimLog("Shared Library: " << fLibraryPath);
    EDepSimLog("Symbol:         " << fSymbolName);
    EDepSim::LogManager::DecreaseIndentation();

    void* fc = EDepSim::GetExternalActionConstructor(fLibraryPath,fSymbolName);
    VKinematicsGenerator* (*constructor)(const char*)
        = reinterpret_cast<VKinematicsGenerator*(*)(const char*)>(fc);
    if (constructor == nullptr) {
        EDepSimError("External generator not found");
        return nullptr;
    }

    return constructor(fUserName.c_str());
}

void EDepSim::ExternalKinematicsFactory::SetNewValue(G4UIcommand* command,
                                                     G4String newValue) {
    if (command == fUserNameCMD) {
        SetName(newValue);
    }
    else if (command == fLibraryPathCMD) {
        SetLibraryPath(newValue);
    }
    else if (command == fSymbolNameCMD) {
        SetSymbolName(newValue);
    }
    else if (command == fExternalKinemCMD) {
        std::string name;
        std::string library;
        std::string symbol;
        std::istringstream is(newValue);
        is >> name >> library >> symbol;
        SetName(name);
        SetLibraryPath(library);
        SetSymbolName(symbol);
    }
    else{
        EDepSimError("Nothing to set the value.");
        EDepSimThrow("Invalid New Value Error");
    }
}
