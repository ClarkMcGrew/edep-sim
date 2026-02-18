#include "EDepSimUserDetectorConstruction.hh"
#include "EDepSimUserPrimaryGeneratorAction.hh"
#include "EDepSimUserRunAction.hh"
#include "EDepSimUserEventAction.hh"
#include "EDepSimUserStackingAction.hh"
#include "EDepSimUserTrackingAction.hh"
#include "EDepSimUserSteppingAction.hh"

#include "EDepSimDetectorMessenger.hh"

#include "EDepSimCreateRunManager.hh"

// The default physics list.
#include "EDepSimPhysicsList.hh"

G4RunManager* EDepSim::CreateRunManager(G4String physicsList) {
    // Set the mandatory initialization classes

    // Construct the default run manager
    G4RunManager* runManager = new G4RunManager;

    // Construct the detector construction class.
    EDepSim::UserDetectorConstruction* theDetector
        = new EDepSim::UserDetectorConstruction;
    runManager->SetUserInitialization(theDetector);

    // Add the physics list first.  This is a G4 requirement!
    runManager->SetUserInitialization(new EDepSim::PhysicsList(physicsList));

    // Set the other mandatory user action class
    runManager->SetUserAction(new EDepSim::UserPrimaryGeneratorAction);
    runManager->SetUserAction(new EDepSim::UserRunAction);
    runManager->SetUserAction(new EDepSim::UserEventAction);
    runManager->SetUserAction(new EDepSim::UserStackingAction);
    runManager->SetUserAction(new EDepSim::UserTrackingAction);

    // Add a break for problems.
    runManager->SetUserAction(new EDepSim::SteppingAction);

    return runManager;
}
