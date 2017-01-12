////////////////////////////////////////////////////////////
// $Id: EDepSim::UserRunActionMessenger.cc,v 1.7 2011/07/19 20:57:43 mcgrew Exp $
//
#include "EDepSimUserRunActionMessenger.hh"
#include "EDepSimUserRunAction.hh"
#include "EDepSimException.hh"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4Timer.hh>

#include <EDepSimLog.hh>

EDepSim::UserRunActionMessenger::UserRunActionMessenger(
    EDepSim::UserRunAction* rdm)
    : fUserRunAction(rdm) { 
    
    fDir = new G4UIdirectory("/edep/random/");
    fDir->SetGuidance("Control of the random seed number.");

    fRandomSeedCmd 
        = new G4UIcmdWithAnInteger("/edep/random/randomSeed",this);
    fRandomSeedCmd->SetGuidance("Sets the random number generator seed"
                                   " using a single integer.");

    fTimeRandomSeedCmd 
        = new G4UIcmdWithoutParameter("/edep/random/timeRandomSeed",this);
    fTimeRandomSeedCmd->SetGuidance("Sets the random number generator seed"
                                    " using the system time.");

    fShowRandomSeedCmd 
        = new G4UIcmdWithoutParameter("/edep/random/showRandomSeed",this);
    fShowRandomSeedCmd->SetGuidance("Show the random number seed.");

    fDetSimRunIdCmd
        = new G4UIcmdWithAnInteger("/edep/runId",this);
    fDetSimRunIdCmd->SetGuidance("This is the first run id that will be used by"
                              " GEANT.  GEANT will automatically increment"
                              " the run id every time it starts a new"
                              " internal run so care must be taken if you"
                              " depend on this to pass-through the run id.");

    fDetSimSubrunIdCmd
        = new G4UIcmdWithAnInteger("/edep/subrunId",this);
    fDetSimSubrunIdCmd->SetGuidance("Set the sub run context field for"
                                 " documentation purposes.  This has no"
                                 " internal meaning.");
}

EDepSim::UserRunActionMessenger::~UserRunActionMessenger() {
    delete fDir;
    delete fRandomSeedCmd;
    delete fTimeRandomSeedCmd;
    delete fShowRandomSeedCmd;
    delete fDetSimRunIdCmd;
    delete fDetSimSubrunIdCmd;
}

void EDepSim::UserRunActionMessenger::SetNewValue(G4UIcommand* command, 
                                              G4String newValue) { 
    if (command == fRandomSeedCmd) {
        long seed = fRandomSeedCmd->GetNewIntValue(newValue);
        fUserRunAction->SetSeed(seed);
    }
    else if (command == fTimeRandomSeedCmd) {
        fUserRunAction->SetTimeSeed();
    }
    else if (command == fShowRandomSeedCmd) {
        long seed = fUserRunAction->GetSeed();
        EDepSimLog("### Random number seed: " << seed);
    }
    else if (command == fDetSimRunIdCmd) {
        int runId = fDetSimRunIdCmd->GetNewIntValue(newValue);
        fUserRunAction->SetDetSimRunId(runId);
    }
    else if (command == fDetSimSubrunIdCmd) {
        int subrunId = fDetSimSubrunIdCmd->GetNewIntValue(newValue);
        fUserRunAction->SetDetSimSubrunId(subrunId);
    }
    else {
        EDepSimThrow("EDepSim::UserRunActionMessenger:: Unimplemented command");
    }
}

G4String EDepSim::UserRunActionMessenger::GetPath() {
    return fDir->GetCommandPath();
}
