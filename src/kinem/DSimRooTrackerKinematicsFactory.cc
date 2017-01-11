#include <TFile.h>
#include <TTree.h>
#include <TBits.h>
#include <TObjString.h>

#include "kinem/DSimRooTrackerKinematicsFactory.hh"
#include "kinem/DSimRooTrackerKinematicsGenerator.hh"

DSimRooTrackerKinematicsFactory::DSimRooTrackerKinematicsFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVKinematicsFactory("rooTracker",parent),
      fInputFile("not-open"), fTreeName("gRooTracker"), 
      fGeneratorName("GENIE"), fOrder("stride"), fFirstEvent(0) {

    fInputFileCMD = new G4UIcmdWithAString(CommandName("input"),this);
    fInputFileCMD->SetGuidance("Set the input file.");
    fInputFileCMD->SetParameterName("name",false);
  
    fTreeNameCMD = new G4UIcmdWithAString(CommandName("tree"),this);
    fTreeNameCMD->SetGuidance("Set the tree path in the input file.");
    fTreeNameCMD->SetParameterName("tree",false);
  
    fGeneratorNameCMD = new G4UIcmdWithAString(CommandName("generator"),this);
    fGeneratorNameCMD->SetGuidance("Set the name of the kinematics source.");
    fGeneratorNameCMD->SetParameterName("generator",false);
  
    fOrderCMD = new G4UIcmdWithAString(CommandName("order"),this);
    fOrderCMD->SetGuidance("Set order that events in the file are used.");
    fOrderCMD->SetParameterName("order",false);
    fOrderCMD->SetCandidates("consecutive stride random");

    fFirstEventCMD = new G4UIcmdWithAnInteger(CommandName("first"),this);
    fFirstEventCMD->SetGuidance("Set the first event to generate.");
    fFirstEventCMD->SetParameterName("number",false);
}

DSimRooTrackerKinematicsFactory::~DSimRooTrackerKinematicsFactory() {
    delete fInputFileCMD;
    delete fTreeNameCMD;
    delete fGeneratorNameCMD;
    delete fOrderCMD;
    delete fFirstEventCMD;
}

void DSimRooTrackerKinematicsFactory::SetNewValue(G4UIcommand* command,
                                                   G4String newValue) {
    if (command == fInputFileCMD) {
        SetInputFile(newValue);
    }
    else if (command == fTreeNameCMD) {
        SetTreeName(newValue);
    }
    else if (command == fGeneratorNameCMD) {
        SetGeneratorName(newValue);
    }
    else if (command == fOrderCMD) {
        SetOrder(newValue);
    }
    else if (command == fFirstEventCMD) {
        SetFirstEvent(fFirstEventCMD->GetNewIntValue(newValue));
    }
}

DSimVKinematicsGenerator* DSimRooTrackerKinematicsFactory::GetGenerator() {
    DSimVKinematicsGenerator* kine
        = new DSimRooTrackerKinematicsGenerator(GetGeneratorName(),
                                                 GetInputFile(), 
                                                 GetTreeName(),
                                                 GetOrder(),
                                                 GetFirstEvent());
    return kine;
}
