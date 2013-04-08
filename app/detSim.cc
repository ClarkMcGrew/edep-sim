//  nd280-mc
//
//  $Id: detsim.cc,v 1.29 2009/08/31 20:07:07 mcgrew Exp $

#include <iostream>
#include <fstream>

#include <csignal>
#include <cstdlib>

#include "TCaptLog.hxx"

#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "DSimCreateRunManager.hh"
#include "DSimPersistencyManager.hh"
#include "DSimRootPersistencyManager.hh"

// The physics lists that can be used.
#include "DSimPhysicsList.hh"

#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

int main(int argc,char** argv) {
    CP::TCaptLog::Configure();

    G4String outputFilename;
    G4String physicsList = "";

    G4int errflg = 0;
    G4int c = 0;
    G4bool useUItcsh = true;
    while (!errflg && ((c=getopt(argc,argv,"p:k:o:tn")) != -1)) {
        switch (c) {
        case 'p':
            physicsList = optarg;
            break;
        case 'o':
            outputFilename = optarg;
            break;
        case 't': // Don't use a tcsh-style command line interface
            useUItcsh = false;
            break;
        default:
            errflg++;
            G4cout << "Option not implemented" << G4endl;
        }
    }

    // Set the mandatory initialization classes
    // Construct the default run manager
    G4RunManager* runManager = DSimCreateRunManager(physicsList);
    
#ifdef G4VIS_USE
    // Visualization.
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif
    
    // Create the persistency manager.
    DSimPersistencyManager* persistencyManager 
        = DSimRootPersistencyManager::GetInstance();

    // Get the pointer to the UI manager
    G4UImanager* UI = G4UImanager::GetUIpointer();

    // Set the defaults for the simulation.
    UI->ApplyCommand("/dsim/control detsim-defaults 1.0");

    if (persistencyManager && outputFilename != "") {
        G4String command = "/db/open ";
        UI->ApplyCommand(command+outputFilename);
    }
    
    std::signal(SIGILL,  SIG_DFL);
    std::signal(SIGBUS,  SIG_DFL);
    std::signal(SIGSEGV, SIG_DFL);

    if (optind < argc) {
        G4String command = "/control/execute ";
        for (int i=optind; i<argc; ++i) {
            G4String macroFilename = argv[i];
            UI->ApplyCommand(command+macroFilename);
        }
    }
    else {
        // G4UIterminal is a (dumb) terminal.
        G4UIsession * session;
        if(useUItcsh) {
            // G4UIterminal is a terminal with tcsh-like control.
            session = new G4UIterminal(new G4UItcsh);
        }
        else {
            session = new G4UIterminal();
        }
        session->SessionStart();
        delete session;
    }
    
    // job termination
#ifdef G4VIS_USE
    delete visManager;
#endif
    
    if (persistencyManager) {
        persistencyManager->Close();
        delete persistencyManager;
    }
    delete runManager;
    
    return 0;
}
