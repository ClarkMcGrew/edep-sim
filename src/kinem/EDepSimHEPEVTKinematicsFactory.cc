#include "kinem/EDepSimHEPEVTKinematicsFactory.hh"
#include "kinem/EDepSimHEPEVTKinematicsGenerator.hh"

EDepSim::HEPEVTKinematicsFactory::HEPEVTKinematicsFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent)
    : EDepSim::VKinematicsFactory("hepevt",parent,false),
      fInputFile("not-open"),
      fFlavorName("pythia"),
      fVerbosity(0),
      fInputFileCMD(NULL),
      fFlavorCMD(NULL),
      fVerboseCMD(NULL) {

    fInputFileCMD = new G4UIcmdWithAString(CommandName("input"),this);
    fInputFileCMD->SetGuidance("Set the input file.");
    fInputFileCMD->SetParameterName("name",false);

    fFlavorCMD = new G4UIcmdWithAString(CommandName("flavor"),this);
    fFlavorCMD->SetGuidance("Choose input format.");
    fFlavorCMD->SetParameterName("flavorname",false);
    fFlavorCMD->SetCandidates("pythia marley");

    fVerboseCMD = new G4UIcmdWithAnInteger(CommandName("verbose"),this);
    fVerboseCMD->SetGuidance("Set verbosity level (0 is default, 2 is max).");
    fVerboseCMD->SetParameterName("number",false);
}

EDepSim::HEPEVTKinematicsFactory::~HEPEVTKinematicsFactory() {
    if (fInputFileCMD) delete fInputFileCMD;
    if (fFlavorCMD)    delete fFlavorCMD;
    if (fVerboseCMD)   delete fVerboseCMD;
}

EDepSim::VKinematicsGenerator*
EDepSim::HEPEVTKinematicsFactory::GetGenerator() {
    EDepSim::VKinematicsGenerator* kine
        = new EDepSim::HEPEVTKinematicsGenerator(GetName(),
                                                 GetInputFile(),
                                                 GetFlavor(),
                                                 GetVerbose());
    return kine;
}

void EDepSim::HEPEVTKinematicsFactory::SetNewValue(G4UIcommand* command,
                                                   G4String newValue) {
    if (command == fInputFileCMD) {
        SetInputFile(newValue);
    }
    else if (command == fFlavorCMD) {
        SetFlavor(newValue);
    }
    else if (command == fVerboseCMD) {
        SetVerbose(fVerboseCMD->GetNewIntValue(newValue));
    }
    else{
        EDepSimError("Nothing to set the value.");
        EDepSimThrow("EDepSim::HEPKinematicsFactory::SetNewValue(): Error");
    }
}
