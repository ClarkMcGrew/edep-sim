//  nd280-mc
//
//  $Id: detsim.cc,v 1.29 2009/08/31 20:07:07 mcgrew Exp $

#include "DSimCreateRunManager.hh"
#include "DSimPersistencyManager.hh"
#include "DSimRootPersistencyManager.hh"
#include "DSimPhysicsList.hh"

#include "TCaptLog.hxx"

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
    CP::TCaptLog::Configure();

    G4String outputFilename;
    G4String physicsList = "";

    G4int errflg = 0;
    G4int c = 0;
    G4bool useUI = false;

    int debugLevel = 0;
    std::map<std::string, CP::TCaptLog::ErrorPriority> namedDebugLevel;

    int logLevel = 1; // Will choose default logging level...
    std::map<std::string, CP::TCaptLog::LogPriority> namedLogLevel;

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
            // Don't use a tcsh-style command line interface
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
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::ErrorLevel;
                    break;
                case 's': case 'S':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::SevereLevel;
                    break;
                case 'w': case 'W':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::WarnLevel;
                    break;
                case 'd': case 'D':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::DebugLevel;
                    break;
                case 't': case 'T':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::TraceLevel;
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
                    namedLogLevel[name.c_str()] = CP::TCaptLog::QuietLevel;
                    break;
                case 'l': case 'L':
                    namedLogLevel[name.c_str()] = CP::TCaptLog::LogLevel;
                    break;
                case 'i': case 'I':
                    namedLogLevel[name.c_str()] = CP::TCaptLog::InfoLevel;
                    break;
                case 'v': case 'V':
                    namedLogLevel[name.c_str()] = CP::TCaptLog::VerboseLevel;
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
        CP::TCaptLog::SetLogLevel(CP::TCaptLog::QuietLevel);
    }
    else if (logLevel == 1) {
        CP::TCaptLog::SetLogLevel(CP::TCaptLog::LogLevel);
        CaptLog("Set log level to LogLevel");
    }
    else if (logLevel == 2) {
        CP::TCaptLog::SetLogLevel(CP::TCaptLog::InfoLevel);
        CaptInfo("Set log level to InfoLevel");
    }
    else if (logLevel >= 3) {
        CP::TCaptLog::SetLogLevel(CP::TCaptLog::VerboseLevel);
        CaptVerbose("Set log level to VerboseLevel");
    }
    
    for (std::map<std::string,CP::TCaptLog::LogPriority>::iterator i 
             = namedLogLevel.begin();
         i != namedLogLevel.end();
         ++i) {
        CP::TCaptLog::SetLogLevel(i->first.c_str(), i->second);
    }
         
    if (debugLevel == 1) {
        CP::TCaptLog::SetDebugLevel(CP::TCaptLog::WarnLevel);
        CaptWarn("Set debug level to WarnLevel");
    }
    else if (debugLevel == 2) {
        CP::TCaptLog::SetDebugLevel(CP::TCaptLog::DebugLevel);
        CaptDebug("Set debug level to DebugLevel");
    }
    else if (debugLevel >= 2) {
        CP::TCaptLog::SetDebugLevel(CP::TCaptLog::TraceLevel);
        CaptTrace("Set debug level to TraceLevel");
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

    if (useUI) {
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        G4String command = "/control/execute ";
        for (int i=optind; i<argc; ++i) {
            G4String macroFilename = argv[i];
            std::cout << "## Run macro: " << macroFilename << std::endl;
            UI->ApplyCommand(command+macroFilename);
        }
        ui->SessionStart();
        delete ui;
    }
    else if (optind < argc) {
        G4String command = "/control/execute ";
        for (int i=optind; i<argc; ++i) {
            G4String macroFilename = argv[i];
            UI->ApplyCommand(command+macroFilename);
        }
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
