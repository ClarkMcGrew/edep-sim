#include "DSimUserDetectorConstruction.hh"
#include "DSimUserPrimaryGeneratorAction.hh"
#include "DSimUserRunAction.hh"
#include "DSimUserEventAction.hh"
#include "DSimUserStackingAction.hh"
#include "DSimUserTrackingAction.hh"
#include "DSimUserSteppingAction.hh"
#include "DSimDetectorMessenger.hh"

#include "DSimCreateRunManager.hh"

// The default physics list.
#include "DSimPhysicsList.hh"

G4RunManager* DSimCreateRunManager(G4String physicsList) {
    // Set the mandatory initialization classes

    // Construct the default run manager
    G4RunManager* runManager = new G4RunManager;
    
    // Construct the detector construction class.
    DSimUserDetectorConstruction* theDetector
        = new DSimUserDetectorConstruction;
    runManager->SetUserInitialization(theDetector);
    
    // Add the physics list first.  This is a G4 requirement!
    runManager->SetUserInitialization(new DSimPhysicsList(physicsList));
    
    // Set the other mandatory user action class
    runManager->SetUserAction(new DSimUserPrimaryGeneratorAction);
    runManager->SetUserAction(new DSimUserRunAction);
    runManager->SetUserAction(new DSimUserEventAction);
    runManager->SetUserAction(new DSimUserStackingAction);
    runManager->SetUserAction(new DSimUserTrackingAction);
    runManager->SetUserAction(new DSimUserSteppingAction);
    
    // Initialize G4 kernel
    // (No longer hardcoded but now done with the command: /run/initialize
    // from macro-files) 
    //runManager->Initialize();

    return runManager;
}
