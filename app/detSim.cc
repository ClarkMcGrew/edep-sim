//  nd280-mc
//
//  $Id: detsim.cc,v 1.29 2009/08/31 20:07:07 mcgrew Exp $

#include "DSimCreateRunManager.hh"
#include "DSimPersistencyManager.hh"
#define USE_ROOT_PERSISTENCY
#ifdef USE_ROOT_PERSISTENCY
#include "DSimRootPersistencyManager.hh"
#endif
#include "DSimLog.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"

#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

#include "globals.hh"

#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>

void usage () {
    std::cout << "Usage: DETSIM.exe [options] [macro]" << std::endl;
    std::cout << "    -h      -- This help message." << std::endl;
    std::cout << "    -p      -- Select the physics list" << std::endl;
    std::cout << "    -o      -- Set the output file" << std::endl;
    std::cout << "    -u      -- Start an interactive run" << std::endl;
    std::cout << "    -d      -- Increase the debug level" << std::endl;
    std::cout << "    -D <name>=[error,severe,warn,debug,trace]"
              << std::endl
              << "            -- Change the named debug level"
              << std::endl;
    std::cout << "    -v      -- Increase the verbosity" << std::endl;
    std::cout << "    -V <name>=[quiet,log,info,verbose]" << std::endl
              << "            -- Change the named log level" 
              << std::endl;
    
    exit(1);
}

int main(int argc,char** argv) {
    DSimLogManager::Configure();

    std::string outputFilename;
    std::string physicsList = "";

    int errflg = 0;
    int c = 0;
    bool useUI = false;

    int debugLevel = 0;
    std::map<std::string, DSimLogManager::ErrorPriority> namedDebugLevel;

    int logLevel = 1; // Will choose default logging level...
    std::map<std::string, DSimLogManager::LogPriority> namedLogLevel;

    while (!errflg && ((c=getopt(argc,argv,"hp:o:udD:qvV:")) != -1)) {
        switch (c) {
        case 'p': {
            physicsList = optarg;
            break;
        }
        case 'o': {
            outputFilename = optarg;
            break;
        }
        case 'u': {
            // Use a tcsh-style command line interface
            useUI = true;
            break;
        }
        case 'd':
        {
            // increase the debugging level.
            ++debugLevel;
            break;
        }
        case 'D':
        {
            // Set the debug level for a named trace.
            std::string arg(optarg);
            std::size_t sep = arg.find("=");
            if (sep != std::string::npos) {
                std::string name = arg.substr(0,sep);
                std::string levelName = arg.substr(sep+1);
                switch (levelName[0]) {
                case 'e': case 'E':
                    namedDebugLevel[name.c_str()] = DSimLogManager::ErrorLevel;
                    break;
                case 's': case 'S':
                    namedDebugLevel[name.c_str()] = DSimLogManager::SevereLevel;
                    break;
                case 'w': case 'W':
                    namedDebugLevel[name.c_str()] = DSimLogManager::WarnLevel;
                    break;
                case 'd': case 'D':
                    namedDebugLevel[name.c_str()] = DSimLogManager::DebugLevel;
                    break;
                case 't': case 'T':
                    namedDebugLevel[name.c_str()] = DSimLogManager::TraceLevel;
                    break;
                default:
                    usage();
                }
            }
            break;
        }
        case 'q':
        {
            // decrease the verbosity level.
            if (logLevel>0) --logLevel;
            else logLevel = 0;
            break;
        }
        case 'v':
        {
            // increase the verbosity level.
            if (logLevel>0) ++logLevel;
            else logLevel = 2;
            break;
        }
        case 'V':
        {
            // Set the debug level for a named trace.
            std::string arg(optarg);
            std::size_t sep = arg.find("=");
            if (sep != std::string::npos) {
                std::string name = arg.substr(0,sep);
                std::string levelName = arg.substr(sep+1);
                switch (levelName[0]) {
                case 'q': case 'Q':
                    namedLogLevel[name.c_str()] = DSimLogManager::QuietLevel;
                    break;
                case 'l': case 'L':
                    namedLogLevel[name.c_str()] = DSimLogManager::LogLevel;
                    break;
                case 'i': case 'I':
                    namedLogLevel[name.c_str()] = DSimLogManager::InfoLevel;
                    break;
                case 'v': case 'V':
                    namedLogLevel[name.c_str()] = DSimLogManager::VerboseLevel;
                    break;
                default:
                    usage();
                }
            }
            break;
        }
        case 'h':
        default:
            usage();
        }
    }

    if (logLevel < 1) {
        DSimLogManager::SetLogLevel(DSimLogManager::QuietLevel);
    }
    else if (logLevel == 1) {
        DSimLogManager::SetLogLevel(DSimLogManager::LogLevel);
        DSimLog("Set log level to LogLevel");
    }
    else if (logLevel == 2) {
        DSimLogManager::SetLogLevel(DSimLogManager::InfoLevel);
        DSimInfo("Set log level to InfoLevel");
    }
    else if (logLevel >= 3) {
        DSimLogManager::SetLogLevel(DSimLogManager::VerboseLevel);
        DSimVerbose("Set log level to VerboseLevel");
    }
    
    for (std::map<std::string,DSimLogManager::LogPriority>::iterator i 
             = namedLogLevel.begin();
         i != namedLogLevel.end();
         ++i) {
        DSimLogManager::SetLogLevel(i->first.c_str(), i->second);
    }
         
    if (debugLevel == 1) {
        DSimLogManager::SetDebugLevel(DSimLogManager::WarnLevel);
        DSimWarn("Set debug level to WarnLevel");
    }
    else if (debugLevel == 2) {
        DSimLogManager::SetDebugLevel(DSimLogManager::DebugLevel);
        DSimDebug("Set debug level to DebugLevel");
    }
    else if (debugLevel >= 2) {
        DSimLogManager::SetDebugLevel(DSimLogManager::TraceLevel);
        DSimTrace("Set debug level to TraceLevel");
    }

    for (std::map<std::string,DSimLogManager::ErrorPriority>::iterator i 
             = namedDebugLevel.begin();
         i != namedDebugLevel.end();
         ++i) {
        DSimLogManager::SetDebugLevel(i->first.c_str(), i->second);
    }
         
    // Set the mandatory initialization classes
    // Construct the default run manager
    G4RunManager* runManager = DSimCreateRunManager(physicsList);
    
#ifdef G4VIS_USE
    // Visualization.
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif
    
    // Create the persistency manager.  The persistency manager must derive
    // from G4VPersistencyManager which will make this object available to the
    // G4RunManager as a singleton.  There can only be one persistency manager
    // at a time.  The Store methods will then be called by the run managers
    // Analyze methods.  The persistency manager doesn't *have* to be derived
    // from DSimRootPersistencyManager, but a lot of the trajectory and hit
    // handling functionality is handled by that class.
    DSimPersistencyManager* persistencyManager = NULL;

#ifdef USE_ROOT_PERSISTENCY
    persistencyManager = new DSimRootPersistencyManager();
#endif
    
    // Create a "no i/o" persistency manager.  This doesn't actually save
    // anything, and it may be better to stop if there isn't a real
    // persistency manager declared.
    if (!persistencyManager) persistencyManager = new DSimPersistencyManager();

    // Get the pointer to the UI manager
    G4UImanager* UI = G4UImanager::GetUIpointer();

    // Set the defaults for the simulation.
    UI->ApplyCommand("/dsim/control detsim-defaults 1.0");

    // Open the file if one was declared on the command line.
    if (persistencyManager && outputFilename != "") {
        std::string command = "/dsim/db/open ";
        UI->ApplyCommand(command+outputFilename);
    }
    
    std::signal(SIGILL,  SIG_DFL);
    std::signal(SIGBUS,  SIG_DFL);
    std::signal(SIGSEGV, SIG_DFL);

    if (useUI) {
        // Make a command line available if one was requested.
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        std::string command = "/control/execute ";
        for (int i=optind; i<argc; ++i) {
            std::string macroFilename = argv[i];
            std::cout << "## Run macro: " << macroFilename << std::endl;
            UI->ApplyCommand(command+macroFilename);
        }
        ui->SessionStart();
        delete ui;
    }
    else if (optind < argc) {
        // Run a macro from the command line.
        std::string command = "/control/execute ";
        for (int i=optind; i<argc; ++i) {
            std::string macroFilename = argv[i];
            UI->ApplyCommand(command+macroFilename);
        }
    }
    
    // job termination
#ifdef G4VIS_USE
    delete visManager;
#endif
    
    // If we have the persistency manager, then make sure it's closed.
    if (persistencyManager) {
        persistencyManager->Close();
        delete persistencyManager;
    }
    delete runManager;
    
    return 0;
}
