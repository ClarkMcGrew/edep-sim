/////////////////////////////////////////////////////////
// $Id: tutRunSimulation.cxx,v 1.17 2011/09/06 18:58:35 mcgrew Exp $


#include <iostream>
#include <sstream>
#include <cmath>

#include <TVector3.h>
#include <TGeoManager.h>

#include <TND280Input.hxx>
#include <TEvent.hxx>
#include <TManager.hxx>

#include "ND280PhysicsList.hh"
#include "tutRunSimulation.hxx"

namespace detsim {
    EventVector Events;
}

#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "ND280CreateRunManager.hh"
#include "ND280PersistencyManager.hh"
#include "ND280RootPersistencyManager.hh"

namespace {
    void GenerateEvent(G4UImanager* ui, 
                  int count,
                  std::string particle,
                  double momentum,
                  double mass,
                  TVector3 pos,
                  TVector3 dir) {
        std::ostringstream command;

        command << "/gps/verbose 2" << std::ends;
        ui->ApplyCommand(command.str());

        command.seekp(std::ios::beg);
        command << "/gps/particle " << particle << std::ends;
        ui->ApplyCommand(command.str());
        
        double energy = std::sqrt(momentum*momentum + mass*mass);
        command.seekp(std::ios::beg);
        command << "/gps/energy " << energy << " MeV" << std::ends;
        ui->ApplyCommand(command.str());
        
        command.seekp(std::ios::beg);
        command << "/gps/pos/type Point" << std::ends;
        ui->ApplyCommand(command.str());
        
        command.seekp(std::ios::beg);
        command << "/gps/position " 
                << pos.X() 
                << " " << pos.Y() 
                << " " << pos.Z() 
                << " cm"
                << std::ends;
        ui->ApplyCommand(command.str());
        
        command.seekp(std::ios::beg);
        command << "/gps/direction " 
                << dir.X() 
                << " " << dir.Y() 
                << " " << dir.Z() 
                << std::ends;
        ui->ApplyCommand(command.str());
        
        command.seekp(std::ios::beg);
        command << "/run/beamOn " << count << std::ends;
        ui->ApplyCommand(command.str());
    }
}

// Generate a ND280MC output file to be available during the tut tests.  This
// must be called in the test constructor, and can be called multiple times.
void GenerateND280MCEvents(std::string caller) {
    if (detsim::Events.size() > 0) {
        return;
    }

    std::cout << "Generate ND280MC Events in test " << caller << std::endl;

    // Set the mandatory initialization classes
    // Construct the default run manager
    G4RunManager* runManager = ND280CreateRunManager("QGSP_BERT");
    
    // get the pointer to the UI manager
    G4UImanager* UI = G4UImanager::GetUIpointer();
    
    // Get a pointer to the persistency manager.
    ND280PersistencyManager* persistencyManager 
        = ND280RootPersistencyManager::GetInstance();

    std::string fileName = "tutND280MC";

    UI->ApplyCommand("/control/verbose 2");
    UI->ApplyCommand("/run/verbose 0");
    UI->ApplyCommand("/event/verbose 0");
    UI->ApplyCommand("/tracking/verbose 0");

    UI->ApplyCommand("/db/open " + fileName);
    UI->ApplyCommand("/dsim/update");

    // Generate events to check that the physics tables exist for these
    // particle types  
    
    // Event 0: 500 MeV/c muon
    GenerateEvent(UI,1,"mu+",500,105, TVector3(0,0,0), TVector3(0,0,1));

    // Event 1: 500 MeV/c positron
    GenerateEvent(UI,1,"e+",500,0.511, TVector3(0,0,0), TVector3(0,0,1));

    // Event 2: 500 MeV/c gamma
    GenerateEvent(UI,1,"gamma",500,0, TVector3(0,0,0), TVector3(0,0,1));

    // Event 3: 500 MeV/c pi minus
    GenerateEvent(UI,1,"pi-",500,138, TVector3(0,0,0), TVector3(0,0,1));

    // Event 4: 500 MeV/c pi plus
    GenerateEvent(UI,1,"pi+",500,138, TVector3(0,0,0), TVector3(0,0,1));

    // Event 5: 500 MeV/c pi zero
    GenerateEvent(UI,1,"pi0",500,134, TVector3(0,0,0), TVector3(0,0,1));

    // Event 6: 1000 MeV/c proton
    GenerateEvent(UI,1,"proton",1000,938, TVector3(0,0,0), TVector3(0,0,1));

    // Event 7: 1000 MeV/c neutron
    GenerateEvent(UI,1,"neutron",1000,940, TVector3(0,0,0), TVector3(0,0,1));

    //////////////////////////////////
    // Check that the trajectory thresholds are working.
    //////////////////////////////////

    // Event 8: 1000 MeV/c positron -- with high gamma threshold 
    persistencyManager->SetGammaThreshold(50*MeV);
    GenerateEvent(UI,1,"e+",1000,0.511, TVector3(0,0,0), TVector3(0,0,1));

    // Event 9: 1000 MeV/c positron -- With low gamma threshold
    persistencyManager->SetGammaThreshold(5*MeV);
    GenerateEvent(UI,1,"e+",1000,0.511, TVector3(0,0,0), TVector3(0,0,1));

    //////////////////////////////////
    // Check that the primary gamma is saved even if it is below the cut
    // thresholds.
    //////////////////////////////////

    // Event 10: 50 MeV/c gamma -- with high gamma threshold.
    persistencyManager->SetGammaThreshold(100*MeV);
    GenerateEvent(UI,1,"gamma",50,0, TVector3(0,0,0), TVector3(0,0,1));

    persistencyManager->Close();
    delete persistencyManager;
    delete runManager;

    // Read the same events from the file and save them in a local vector.
    fileName += ".root";
    CP::TND280Input* input = new CP::TND280Input(fileName.c_str(),"OLD");
    for (CP::TEvent* event = input->FirstEvent();
         event != NULL;
         event = input->NextEvent()) {
        std::cout << "Read Event " << event->GetContext().GetRun() 
                  << "." << event->GetContext().GetEvent()
                  << std::endl;
        CP::TManager::Get().Geometry();
        detsim::Events.push_back(event);
    }
    std::cout << "Read ND280MC file: " << fileName << std::endl;
    input->Close();
    delete input;
}
