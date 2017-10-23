#include "TEventDisplay.hxx"
#include "TEventChangeManager.hxx"

#include <TFile.h>

#include <TROOT.h>
#include <TSystem.h>
#include <TApplication.h>

#include <getopt.h>

#include <iostream>
#include <memory>

void usage() {
    std::cout << "Usage: edep-disp [options] [input-file] " << std::endl;
    std::cout << "  The edep-sim event display: " << std::endl;
    std::cout << "   -g    Toggle showing the geometry (on by default)."
              << std::endl;
    std::cout << "   -s<name>  Show volumes containing <name> (limit this to "
              << std::endl
              << "             a few, or it's horribly slow."
              << std::endl;
    std::cout << "   -S    Clear the volumes that will be shown."
              << std::endl;
    std::cout << "   -h    This message." << std::endl;
}

int main(int argc, char **argv) {
    std::string fileName = "";
    bool showGeometry = true;
    std::vector<std::string> volumesToShow;
    
    while (1) {
        int c = getopt(argc, argv, "?hgs:S");
        if (c == -1) break;
        switch (c) {
        case 'g': {
            // Show the geometry.
            showGeometry = not showGeometry;
            break;
        }
        case 's': {
            volumesToShow.push_back(optarg);
            break;
        }
        case 'S': {
            volumesToShow.clear();
            break;
        }
        case '?':
        case 'h':
        default:
            usage();
            return 1;
        }
    }
    
    // Check if there is an input file on the command line.
    if (argc - optind > 0) {
        fileName = argv[optind];
    }

    TFile* eventSource = NULL;
    if (!fileName.empty()) {
        eventSource = new TFile(fileName.c_str());
    }
    if (!eventSource) {
        usage();
        return 1;
    }

    TApplication theApp("EventDisplay", 0, 0);
    theApp.ExitOnException(TApplication::kExit);

    EDep::TEventDisplay& ev = EDep::TEventDisplay::Get();
    ev.EventChange().SetShowGeometry(showGeometry);
    for (std::vector<std::string>::iterator v = volumesToShow.begin();
         v != volumesToShow.end(); ++v) {
        ev.EventChange().AddVolumeToShow(*v);
    }
    ev.EventChange().SetEventSource(eventSource);
    
    theApp.Run(kFALSE);

    return 0;
}


