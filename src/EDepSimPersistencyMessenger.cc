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
        "Control which primaries have saved trajectories.\n"
        "  True: Save all prim. part. trajectories.\n"
        "  False: Save prim. that ultimately deposit energy in SD.");

    fRequireEventsWithHitsCMD
        = new G4UIcmdWithABool("/edep/db/set/requireEventsWithHits", this);
    fRequireEventsWithHitsCMD->SetGuidance(
        "If true, don't save primaries/trajectories for events that\n"
        "  have no hits in sensitive detectors");

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
        "Add a Perl RegExp for a phys. vol. boundary where a\n"
        "    trajectory point is saved. The expression is compared to a\n"
        "    string constructed \":particle:charge:volume:\" where particle\n"
        "    is the particle name, charge is \"charged\" or \"neutral\" and\n"
        "    volume is the physical volume name.");
    fTrajectoryBoundaryCMD->SetParameterName("boundary",true);
    fTrajectoryBoundaryCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fTrajectoryBulkCMD
        = new G4UIcmdWithAString("/edep/db/set/trajectoryBulk",this);
    fTrajectoryBulkCMD->SetGuidance(
        "Add a Perl RegExp for a phys. vol. (bulk) where a\n"
        "    trajectory point is saved. The expression is compared to a\n"
        "    string constructed \":particle:charge:volume:\" where particle\n"
        "    is the particle name, charge is \"charged\" or \"neutral\" and\n"
        "    volume is the physical volume name.");
    fTrajectoryBulkCMD->SetParameterName("bulk",true);
    fTrajectoryBulkCMD->AvailableForStates(G4State_PreInit,G4State_Idle);

    fClearBoundariesCMD
        = new G4UIcmdWithoutParameter("/edep/db/set/clearBoundaries",this);
    fClearBoundariesCMD->SetGuidance("Remove all of the boundaries for "
                                     "trajectory points.");

}

EDepSim::PersistencyMessenger::~PersistencyMessenger() {
    delete fOpenCMD;
    delete fCloseCMD;
    delete fGammaThresholdCMD;
    delete fNeutronThresholdCMD;
    delete fLengthThresholdCMD;
    delete fSaveAllPrimaryTrajectoriesCMD;
    delete fTrajectoryPointAccuracyCMD;
    delete fTrajectoryPointDepositCMD;
    delete fTrajectoryBoundaryCMD;
    delete fTrajectoryBulkCMD;
    delete fClearBoundariesCMD;
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
    else if (command == fRequireEventsWithHitsCMD) {
        fPersistencyManager->SetRequireEventsWithHits(
            fRequireEventsWithHitsCMD->GetNewBoolValue(newValue));
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

    else if (command == fTrajectoryBulkCMD) {
        fPersistencyManager->AddTrajectoryBulk(newValue);
    }
    else if (command == fClearBoundariesCMD) {
        fPersistencyManager->ClearTrajectoryBoundaries();
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
    else if (command==fRequireEventsWithHitsCMD) {
        currentValue = fRequireEventsWithHitsCMD->ConvertToString(
            fPersistencyManager->GetRequireEventsWithHits());
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
