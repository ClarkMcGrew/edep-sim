#include "globals.hh"

#include "EDepSimCreatePhysicsListMessenger.hh"
#include "EDepSimPhysicsList.hh"
#include "EDepSimExtraPhysics.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4SystemOfUnits.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

EDepSim::CreatePhysicsListMessenger::CreatePhysicsListMessenger(G4VModularPhysicsList* physList, EDepSim::ExtraPhysics* extra)
  : fPhysicsList(physList)
  , fExtraPhysics(extra)
{

    fDirectory = new G4UIdirectory("/edep/phys/");
    fDirectory->SetGuidance("Control the physics lists");

    fGammaCutCMD = new G4UIcmdWithADoubleAndUnit("/edep/phys/gammaCut",this);
    fGammaCutCMD->SetGuidance("Set gamma cut");
    fGammaCutCMD->SetParameterName("Gcut",false);
    fGammaCutCMD->SetUnitCategory("Length");
    fGammaCutCMD->SetRange("Gcut>0.0");
    fGammaCutCMD->SetDefaultUnit("mm");
    fGammaCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fElectCutCMD = new G4UIcmdWithADoubleAndUnit("/edep/phys/electronCut",
                                                 this);
    fElectCutCMD->SetGuidance("Set electron cut");
    fElectCutCMD->SetParameterName("Ecut",false);
    fElectCutCMD->SetUnitCategory("Length");
    fElectCutCMD->SetRange("Ecut>0.0");
    fElectCutCMD->SetDefaultUnit("mm");
    fElectCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fPosCutCMD = new G4UIcmdWithADoubleAndUnit("/edep/phys/positronCut",
                                               this);
    fPosCutCMD->SetGuidance("Set positron cut");
    fPosCutCMD->SetParameterName("Pcut",false);
    fPosCutCMD->SetUnitCategory("Length");
    fPosCutCMD->SetRange("Pcut>0.0");
    fPosCutCMD->SetDefaultUnit("mm");
    fPosCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fAllCutCMD = new G4UIcmdWithADoubleAndUnit("/edep/phys/allCuts",this);
    fAllCutCMD->SetGuidance("Set cut for all");
    fAllCutCMD->SetParameterName("cut",false);
    fAllCutCMD->SetUnitCategory("Length");
    fAllCutCMD->SetRange("cut>0.0");
    fAllCutCMD->SetDefaultUnit("mm");
    fAllCutCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fIonizationModelCMD = new G4UIcmdWithABool("/edep/phys/ionizationModel",
                                                    this);
    fIonizationModelCMD->SetGuidance("Set ionization model in the LAr");
    fIonizationModelCMD->SetParameterName("fraction",false);
    fIonizationModelCMD->AvailableForStates(G4State_PreInit,G4State_Idle);
}

EDepSim::CreatePhysicsListMessenger::~CreatePhysicsListMessenger() {
    delete fGammaCutCMD;
    delete fElectCutCMD;
    delete fPosCutCMD;
    delete fAllCutCMD;
    delete fIonizationModelCMD;
}

void EDepSim::CreatePhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue) {
    if (command == fGammaCutCMD) {
        G4double cut = fGammaCutCMD->GetNewDoubleValue(newValue);
        G4cout << "#### RWH ## set Gamma cut to " << cut/mm
               << " [mm]" << G4endl;
        fPhysicsList->SetCutValue(cut, "gamma"); // G4Gamma::Gamma());
    }
    else if (command == fElectCutCMD) {
        G4double cut = fElectCutCMD->GetNewDoubleValue(newValue);
        G4cout << "#### RWH ## set Electron cut to " << cut/mm
               << " [mm]"<< G4endl;
        fPhysicsList->SetCutValue(cut, "e-"); //G4Electron::Electron());
    }
    else if (command == fPosCutCMD) {
        G4double cut = fPosCutCMD->GetNewDoubleValue(newValue);
        G4cout << "#### RWH ## set Positron cut to " << cut/mm
               << " [mm]" << G4endl;
        fPhysicsList->SetCutValue(cut, "e+"); //G4Positron::Positron());
    }
    else if (command == fAllCutCMD) {
        G4double cut = fAllCutCMD->GetNewDoubleValue(newValue);
        G4cout << "#### RWH ## set Gamma,Electron,Positron cut to " << cut/mm
               << " [mm]" << G4endl;
        fPhysicsList->SetCutValue(cut, "gamma"); //G4Gamma::Gamma());
        fPhysicsList->SetCutValue(cut, "e-"); //G4Electron::Electron());
        fPhysicsList->SetCutValue(cut, "e+"); //G4Positron::Positron());
    }
    else if (command == fIonizationModelCMD) {
        G4bool ionmodel = fIonizationModelCMD->GetNewBoolValue(newValue);
        G4cout << "#### RWH ## set IonizationModel "
               << (ionmodel?"true":"false") << G4endl;
        fExtraPhysics->SetIonizationModel(ionmodel);
    }
}
