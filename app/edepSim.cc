#include "EDepSimCreateRunManager.hh"
#include "EDepSimPersistencyManager.hh"
#define USE_ROOT_PERSISTENCY
#ifdef USE_ROOT_PERSISTENCY
#include "EDepSimRootPersistencyManager.hh"
#endif
#include "EDepSimLog.hh"

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
    std::cout << "    -d      -- Increase the debug level" << std::endl;
    std::cout << "    -D <name>=[error,severe,warn,debug,trace]"
              << std::endl
              << "            -- Change the named debug level"
              << std::endl;
    std::cout << "    -g      -- Set a GDML file" << std::endl;
    std::cout << "    -o      -- Set the output file" << std::endl;
    std::cout << "    -p      -- Select the physics list" << std::endl;
    std::cout << "    -u      -- Start an interactive run" << std::endl;
    std::cout << "    -v      -- Increase the verbosity" << std::endl;
    std::cout << "    -V <name>=[quiet,log,info,verbose]" << std::endl
              << "            -- Change the named log level" 
              << std::endl;
    std::cout << "    -h      -- This help message." << std::endl;
    
    exit(1);
}

int main(int argc,char** argv) {
    EDepSim::LogManager::Configure();

    std::string outputFilename;
    std::string physicsList = "";
    std::string gdmlFilename = "";

    int errflg = 0;
    int c = 0;
    bool useUI = false;

    int debugLevel = 0;
    std::map<std::string, EDepSim::LogManager::ErrorPriority> namedDebugLevel;

    int logLevel = 1; // Will choose default logging level...
    std::map<std::string, EDepSim::LogManager::LogPriority> namedLogLevel;

    while (!errflg && ((c=getopt(argc,argv,"dD:g:o:p:quvV:h")) != -1)) {
        switch (c) {
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
                    namedDebugLevel[name.c_str()]
                        = EDepSim::LogManager::ErrorLevel;
                    break;
                case 's': case 'S':
                    namedDebugLevel[name.c_str()]
                        = EDepSim::LogManager::SevereLevel;
                    break;
                case 'w': case 'W':
                    namedDebugLevel[name.c_str()]
                        = EDepSim::LogManager::WarnLevel;
                    break;
                case 'd': case 'D':
                    namedDebugLevel[name.c_str()]
                        = EDepSim::LogManager::DebugLevel;
                    break;
                case 't': case 'T':
                    namedDebugLevel[name.c_str()]
                        = EDepSim::LogManager::TraceLevel;
                    break;
                default:
                    usage();
                }
            }
            break;
        }
        case 'g': {
            gdmlFilename = optarg;
            break;
        }
        case 'o': {
            outputFilename = optarg;
            break;
        }
        case 'p': {
            physicsList = optarg;
            break;
        }
        case 'u': {
            // Use a tcsh-style command line interface
            useUI = true;
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
                    namedLogLevel[name.c_str()]
                        = EDepSim::LogManager::QuietLevel;
                    break;
                case 'l': case 'L':
                    namedLogLevel[name.c_str()]
                        = EDepSim::LogManager::LogLevel;
                    break;
                case 'i': case 'I':
                    namedLogLevel[name.c_str()]
                        = EDepSim::LogManager::InfoLevel;
                    break;
                case 'v': case 'V':
                    namedLogLevel[name.c_str()]
                        = EDepSim::LogManager::VerboseLevel;
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
        EDepSim::LogManager::SetLogLevel(EDepSim::LogManager::QuietLevel);
    }
    else if (logLevel == 1) {
        EDepSim::LogManager::SetLogLevel(EDepSim::LogManager::LogLevel);
        EDepSimLog("Set log level to LogLevel");
    }
    else if (logLevel == 2) {
        EDepSim::LogManager::SetLogLevel(EDepSim::LogManager::InfoLevel);
        EDepSimInfo("Set log level to InfoLevel");
    }
    else if (logLevel >= 3) {
        EDepSim::LogManager::SetLogLevel(EDepSim::LogManager::VerboseLevel);
        EDepSimVerbose("Set log level to VerboseLevel");
    }
    
    for (std::map<std::string,EDepSim::LogManager::LogPriority>::iterator i 
             = namedLogLevel.begin();
         i != namedLogLevel.end();
         ++i) {
        EDepSim::LogManager::SetLogLevel(i->first.c_str(), i->second);
    }
         
    if (debugLevel == 1) {
        EDepSim::LogManager::SetDebugLevel(EDepSim::LogManager::WarnLevel);
        EDepSimWarn("Set debug level to WarnLevel");
    }
    else if (debugLevel == 2) {
        EDepSim::LogManager::SetDebugLevel(EDepSim::LogManager::DebugLevel);
        EDepSimDebug("Set debug level to DebugLevel");
    }
    else if (debugLevel >= 2) {
        EDepSim::LogManager::SetDebugLevel(EDepSim::LogManager::TraceLevel);
        EDepSimTrace("Set debug level to TraceLevel");
    }

    for (std::map<std::string,EDepSim::LogManager::ErrorPriority>::iterator i 
             = namedDebugLevel.begin();
         i != namedDebugLevel.end();
         ++i) {
        EDepSim::LogManager::SetDebugLevel(i->first.c_str(), i->second);
    }
         
    // Set the mandatory initialization classes
    // Construct the default run manager
    G4RunManager* runManager = EDepSim::CreateRunManager(physicsList);
    
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
    // from EDepSim::RootPersistencyManager, but a lot of the trajectory and hit
    // handling functionality is handled by that class.
    EDepSim::PersistencyManager* persistencyManager = NULL;

#ifdef USE_ROOT_PERSISTENCY
    persistencyManager = new EDepSim::RootPersistencyManager();
#endif
    
    // Create a "no i/o" persistency manager.  This doesn't actually save
    // anything, and it may be better to stop if there isn't a real
    // persistency manager declared.
    if (!persistencyManager) {
        persistencyManager = new EDepSim::PersistencyManager();
    }

    // Get the pointer to the UI manager
    G4UImanager* UI = G4UImanager::GetUIpointer();

    // Open the file if one was declared on the command line.
    if (gdmlFilename != "") {
        std::string command = "/edep/gdml/read ";
        UI->ApplyCommand(command+gdmlFilename);
    }
    
    // Set the defaults for the simulation.
    UI->ApplyCommand("/edep/control edepsim-defaults 1.0");

    // Open the file if one was declared on the command line.
    if (persistencyManager && outputFilename != "") {
        std::string command = "/edep/db/open ";
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
