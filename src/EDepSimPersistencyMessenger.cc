////////////////////////////////////////////////////////////
//

#include "EDepSimPersistencyMessenger.hh"
#include "EDepSimPersistencyManager.hh"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcommand.hh>
#include <G4ios.hh>

EDepSim::PersistencyMessenger::PersistencyMessenger(
    EDepSim::PersistencyManager* persistencyMgr)
    : fPersistencyManager(persistencyMgr) {
    fPersistencyDIR = new G4UIdirectory("/edep/db/");
    fPersistencyDIR->SetGuidance("Output file control commands.");

    fOpenCMD = new G4UIcmdWithAString("/edep/db/open",this);
    fOpenCMD->SetGuidance("Set the name of the output file and open it.");
    fOpenCMD->SetParameterName("filename",true);
    fOpenCMD->SetDefaultValue("edepsim-output.root");
    fOpenCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fCloseCMD = new G4UIcmdWithoutParameter("/edep/db/close",this);
    fCloseCMD->SetGuidance("Close the output file.");

    fPersistencySetDIR = new G4UIdirectory("/edep/db/set/");
    fPersistencySetDIR->SetGuidance("Set various parameters");

    fGammaThresholdCMD
        = new G4UIcmdWithADoubleAndUnit("/edep/db/set/gammaThreshold", this);
    fGammaThresholdCMD->SetGuidance(
        "Set momentum threshold for writing out gamma-ray trajectories");
    fGammaThresholdCMD->SetParameterName("momentum", false, false);
    fGammaThresholdCMD->SetUnitCategory("Energy");

    fNeutronThresholdCMD
        = new G4UIcmdWithADoubleAndUnit("/edep/db/set/neutronThreshold", this);
    fNeutronThresholdCMD->SetGuidance(
        "Set momentum threshold for writing out neutron trajectories");
    fNeutronThresholdCMD->SetParameterName("momentum", false, false);
    fNeutronThresholdCMD->SetUnitCategory("Energy");

    fLengthThresholdCMD
        = new G4UIcmdWithADoubleAndUnit("/edep/db/set/lengthThreshold", this);
    fLengthThresholdCMD->SetGuidance(
        "Set length of track in an SD for writing out particle trajectories");
    fLengthThresholdCMD->SetParameterName("length", false, false);
    fLengthThresholdCMD->SetUnitCategory("Length");

    fSaveAllPrimaryTrajectoriesCMD
        = new G4UIcmdWithABool("/edep/db/set/saveAllPrimTraj", this);
    fSaveAllPrimaryTrajectoriesCMD->SetGuidance(
        "Control which primaries have saved trajectories --"
        " True: Save all prim. part. trajectories."
        " False: Save prim. that ultimately deposit energy in SD.");

    fSaveAllTrajectoriesCMD
        = new G4UIcmdWithADoubleAndUnit("/edep/db/set/saveAllTraj", this);
    fSaveAllTrajectoriesCMD->SetGuidance(
        "If positive, then save all trajectories with more than this"
        " much energy deposit. If negative, the save all trajectories."
        " Use with care since the file becomes large.");
    fSaveAllTrajectoriesCMD->SetParameterName("energy", false, false);
    fSaveAllTrajectoriesCMD->SetUnitCategory("Energy");
    fSaveAllTrajectoriesCMD->SetDefaultValue(-1.0);

    fTrajectoryPointAccuracyCMD
        = new G4UIcmdWithADoubleAndUnit("/edep/db/set/trajectoryAccuracy", this);
    fTrajectoryPointAccuracyCMD->SetGuidance(
        "Set the minimum accuracy of the trajectory.");
    fTrajectoryPointAccuracyCMD->SetParameterName("length", false, false);
    fTrajectoryPointAccuracyCMD->SetUnitCategory("Length");

    fTrajectoryPointDepositCMD
        = new G4UIcmdWithADoubleAndUnit("/edep/db/set/trajectoryDeposit", this);
    fTrajectoryPointDepositCMD->SetGuidance(
        "Set the minimum energy deposit for a trajectory point.");
    fTrajectoryPointDepositCMD->SetParameterName("energy", false, false);
    fTrajectoryPointDepositCMD->SetUnitCategory("Energy");

    fTrajectoryBoundaryCMD
        = new G4UIcmdWithAString("/edep/db/set/trajectoryBoundary",this);
    fTrajectoryBoundaryCMD->SetGuidance(
        "Add a Perl RegExp for a phys. vol. boundary where a"
        " trajectory point is saved. The expression is compared to a"
        " string constructed \":particle:charge:volume:\" where particle"
        " is the particle name, charge is \"charged\" or \"neutral\" and"
        " volume is the physical volume name.");
    fTrajectoryBoundaryCMD->SetParameterName("boundary",true);
    fTrajectoryBoundaryCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fClearBoundariesCMD
        = new G4UIcmdWithoutParameter("/edep/db/set/clearBoundaries",this);
    fClearBoundariesCMD->SetGuidance("Remove all of the boundaries for"
                                     " trajectory points.");

    fTrajectoryPointRuleCMD
        = new G4UIcommand("/edep/db/set/trajectoryRule",this);
    fTrajectoryPointRuleCMD->SetGuidance(
        "Add a rule to save a trajectory point.");
    fTrajectoryPointRuleCMD->AvailableForStates(
        G4State_PreInit,G4State_Idle);
    G4UIparameter* param = new G4UIparameter("process",'i',false);
    param->SetGuidance(
        "Select trajectory points with this process."
        " The process numbers are defined by geant"
        " in G4ProcessType.h."
        " A value of -1 specifies all processes.");
    fTrajectoryPointRuleCMD->SetParameter(param);
    param = new G4UIparameter("subprocess",'i',true);
    param->SetGuidance(
        "Select points with this subprocess."
        " The subprocess numbers are defined in several"
        " include files, notably"
        " G4EmProcessSubType.hh,"
        " G4OpProcessSubType.hh,"
        " and G4HadronicProcessType.hh."
        " A value of -1 specifies all subprocesses.");
    param->SetDefaultValue(-1);
    fTrajectoryPointRuleCMD->SetParameter(param);
    param = new G4UIparameter("threshold",'d',true);
    param->SetGuidance("Select points with more than this energy deposit.");
    param->SetDefaultValue(-1.0);
    fTrajectoryPointRuleCMD->SetParameter(param);
    param = new G4UIparameter("energy unit",'s',true);
    param->SetDefaultValue("MeV");
    fTrajectoryPointRuleCMD->SetParameter(param);

    fClearTrajectoryPointRulesCMD
        = new G4UIcmdWithoutParameter("/edep/db/set/clearTrajectoryRules",this);
    fClearTrajectoryPointRulesCMD->SetGuidance(
        "Clear all of the trajectory point"
        " save rules.");

}

EDepSim::PersistencyMessenger::~PersistencyMessenger() {
    delete fOpenCMD;
    delete fCloseCMD;
    delete fGammaThresholdCMD;
    delete fNeutronThresholdCMD;
    delete fLengthThresholdCMD;
    delete fSaveAllPrimaryTrajectoriesCMD;
    delete fSaveAllTrajectoriesCMD;
    delete fTrajectoryPointAccuracyCMD;
    delete fTrajectoryPointDepositCMD;
    delete fTrajectoryBoundaryCMD;
    delete fClearBoundariesCMD;
    delete fTrajectoryPointRuleCMD;
    delete fClearTrajectoryPointRulesCMD;
    delete fPersistencyDIR;
    delete fPersistencySetDIR;
}


void EDepSim::PersistencyMessenger::SetNewValue(G4UIcommand* command,
                                            G4String newValue) {
    if (command==fOpenCMD) {
        fPersistencyManager->Open(newValue);
    }
    else if (command == fCloseCMD) {
        fPersistencyManager->Close();
    }
    else if (command == fGammaThresholdCMD) {
        fPersistencyManager->SetGammaThreshold(
            fGammaThresholdCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fNeutronThresholdCMD) {
        fPersistencyManager->SetNeutronThreshold(
            fNeutronThresholdCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fLengthThresholdCMD) {
        fPersistencyManager->SetLengthThreshold(
            fLengthThresholdCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fSaveAllPrimaryTrajectoriesCMD) {
        fPersistencyManager->SetSaveAllPrimaryTrajectories(
            fSaveAllPrimaryTrajectoriesCMD->GetNewBoolValue(newValue));
    }
    else if (command == fSaveAllTrajectoriesCMD) {
        fPersistencyManager->SetSaveAllTrajectories(
            fSaveAllTrajectoriesCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fTrajectoryPointAccuracyCMD) {
        fPersistencyManager->SetTrajectoryPointAccuracy(
            fTrajectoryPointAccuracyCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fTrajectoryPointDepositCMD) {
        fPersistencyManager->SetTrajectoryPointDeposit(
            fTrajectoryPointDepositCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fTrajectoryBoundaryCMD) {
        fPersistencyManager->AddTrajectoryBoundary(newValue);
    }
    else if (command == fClearBoundariesCMD) {
        fPersistencyManager->ClearTrajectoryBoundaries();
    }
    else if (command == fTrajectoryPointRuleCMD) {
        int process;
        int subprocess;
        double threshold;
        std::string unit;
        std::istringstream val(newValue);
        val >> process >> subprocess >> threshold >> unit;
        fPersistencyManager->AddTrajectoryPointRule(
            process,subprocess,threshold*G4UIcommand::ValueOf(unit.c_str()));
    }
    else if (command == fClearTrajectoryPointRulesCMD) {
        fPersistencyManager->ClearTrajectoryPointRules();
    }
}


G4String EDepSim::PersistencyMessenger::GetCurrentValue(G4UIcommand * command) {
    G4String currentValue;

    if (command==fOpenCMD) {
        currentValue = fPersistencyManager->GetFilename();
    }
    else if (command==fGammaThresholdCMD) {
        currentValue = fGammaThresholdCMD->ConvertToString(
            fPersistencyManager->GetGammaThreshold());
    }
    else if (command==fNeutronThresholdCMD) {
        currentValue = fNeutronThresholdCMD->ConvertToString(
            fPersistencyManager->GetNeutronThreshold());
    }
    else if (command==fLengthThresholdCMD) {
        currentValue = fLengthThresholdCMD->ConvertToString(
            fPersistencyManager->GetLengthThreshold());
    }
    else if (command==fSaveAllPrimaryTrajectoriesCMD) {
        currentValue = fSaveAllPrimaryTrajectoriesCMD->ConvertToString(
            fPersistencyManager->GetSaveAllPrimaryTrajectories());
    }
    else if (command==fSaveAllTrajectoriesCMD) {
        currentValue = fSaveAllTrajectoriesCMD->ConvertToString(
            fPersistencyManager->GetSaveAllTrajectories());
    }
    else if (command==fTrajectoryPointAccuracyCMD) {
        currentValue = fTrajectoryPointAccuracyCMD->ConvertToString(
            fPersistencyManager->GetTrajectoryPointAccuracy());
    }
    else if (command==fTrajectoryPointDepositCMD) {
        currentValue = fTrajectoryPointDepositCMD->ConvertToString(
            fPersistencyManager->GetTrajectoryPointDeposit());
    }

    return currentValue;
}
