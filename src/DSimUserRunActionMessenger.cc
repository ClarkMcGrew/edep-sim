////////////////////////////////////////////////////////////
// $Id: DSimUserRunActionMessenger.cc,v 1.7 2011/07/19 20:57:43 mcgrew Exp $
//
#include "DSimUserRunActionMessenger.hh"
#include "DSimUserRunAction.hh"
#include "DSimException.hh"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4Timer.hh>

#include <DSimLog.hh>

DSimUserRunActionMessenger::DSimUserRunActionMessenger(
    DSimUserRunAction* rdm)
    : fUserRunAction(rdm) { 
    
    fDir = new G4UIdirectory("/dsim/random/");
    fDir->SetGuidance("Control of the random seed number.");

    fRandomSeedCmd 
        = new G4UIcmdWithAnInteger("/dsim/random/randomSeed",this);
    fRandomSeedCmd->SetGuidance("Sets the random number generator seed"
                                   " using a single integer.");

    fTimeRandomSeedCmd 
        = new G4UIcmdWithoutParameter("/dsim/random/timeRandomSeed",this);
    fTimeRandomSeedCmd->SetGuidance("Sets the random number generator seed"
                                    " using the system time.");

    fShowRandomSeedCmd 
        = new G4UIcmdWithoutParameter("/dsim/random/showRandomSeed",this);
    fShowRandomSeedCmd->SetGuidance("Show the random number seed.");

    fDetSimRunIdCmd
        = new G4UIcmdWithAnInteger("/dsim/runId",this);
    fDetSimRunIdCmd->SetGuidance("This is the first run id that will be used by"
                              " GEANT.  GEANT will automatically increment"
                              " the run id every time it starts a new"
                              " internal run so care must be taken if you"
                              " depend on this to pass-through the run id.");

    fDetSimSubrunIdCmd
        = new G4UIcmdWithAnInteger("/dsim/subrunId",this);
    fDetSimSubrunIdCmd->SetGuidance("Set the sub run context field for"
                                 " documentation purposes.  This has no"
                                 " internal meaning.");
}

DSimUserRunActionMessenger::~DSimUserRunActionMessenger() {
    delete fDir;
    delete fRandomSeedCmd;
    delete fTimeRandomSeedCmd;
    delete fShowRandomSeedCmd;
    delete fDetSimRunIdCmd;
    delete fDetSimSubrunIdCmd;
}

void DSimUserRunActionMessenger::SetNewValue(G4UIcommand* command, 
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
        DSimLog("### Random number seed: " << seed);
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
        DSimThrow("DSimUserRunActionMessenger:: Unimplemented command");
    }
}

G4String DSimUserRunActionMessenger::GetPath() {
    return fDir->GetCommandPath();
}
