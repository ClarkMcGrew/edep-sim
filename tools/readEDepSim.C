// TEST MACRO FOR USE WITH OLDER ROOT6.  DOESN"T WORK WHEN CLING KNOWS ABOUT
// THE VARIOUS CLASSES.
#include <TFile.h>
#include <TTree.h>

#include <iostream>

#include <readEDepSim.h>

TTree* gEDepSimTree = NULL;
TG4Event* gEDepSimEvent = NULL;

void readEDepSim() {
    gEDepSimTree = (TTree*) gFile->Get("EDepSimEvents");
    if (!gEDepSimTree) {
        std::cout << "Missing the event tree" << std::endl;
        return;
    }
    
    gEDepSimTree->SetBranchAddress("Event",&gEDepSimEvent);

    gEDepSimTree->Print();
}

TTree* EDepSimTree() {
    return gEDepSimTree;
}

TG4Event* EDepSimEvent() {
    return gEDepSimEvent;
}


void EDepSimDumpEvent() {
    TG4Event* event = EDepSimEvent();
    if (!event) {
        std::cout << "Event not available" << std::endl;
    }
    
    std::cout << " event " << event->EventId;
    std::cout << " primaries " << event->Primaries.size();
    std::cout << " trajectories " << event->Trajectories.size();
    std::cout << std::endl;
    for (std::vector<TG4PrimaryVertex>::iterator
             v = event->Primaries.begin();
         v != event->Primaries.end(); ++v) {
        std::cout << "   Position " << v->Position.Z()
                  << " " << v->Position.Y()
                  << " " << v->Position.Z()
                  << " " << v->Position.T();
        std::cout << "  particles " << v->Particles.size();
        for (std::vector<TG4PrimaryParticle>::iterator
                 p = v->Particles.begin();
             p != v->Particles.end(); ++p) {
            std::cout << " " << p->Name;
        }
        std::cout << endl;
    }
    for (std::vector<TG4Trajectory>::iterator
             t = event->Trajectories.begin();
         t != event->Trajectories.end(); ++t) {
        std::cout << "   Traj " << t->TrackId;
        std::cout << " " << t->ParentId;
        std::cout << " " << t->Name;
        std::cout << " " << t->Points.size();
        int count = 3;
        std::cout << " Up to " << count << " points";
        std::cout << std::endl;
        for (std::vector<TG4TrajectoryPoint>::iterator
                 p = t->Points.begin();
             p != t->Points.end();
             ++p) {
            std::cout << "      ";
            std::cout << " Time: " << p->Position.T();
            std::cout << " Process: " << p->Process;
            std::cout << " Subprocess: " << p->Subprocess;
            std::cout << std::endl;
            if (--count < 1) break;
        }
    }
    for (auto d = event->SegmentDetectors.begin();
         d != event->SegmentDetectors.end(); ++d) {
        std::cout << "   det " << d->first;
        std::cout << " " << d->second.size();
        int count = 10;
        std::cout << " up to " << count << " segments";
        std::cout << std::endl;
        for (std::vector<TG4HitSegment>::iterator
                 h = d->second.begin();
             h != d->second.end();
             ++h) {
            std::cout << "      ";
            std::cout << " P: " << h->PrimaryId;
            std::cout << " E: " << h->EnergyDeposit;
            std::cout << " S: " << h->SecondaryDeposit;
            std::cout << " C: " << h->Contrib.size();
            std::cout << " L: " << h->TrackLength;
            std::cout << std::endl;
            if ((--count) < 1) break;
        }
    }
}

TG4Event* EDepSimEvent(int i) {
    if (!gEDepSimTree) {
        std::cout << "Missing the event tree" << std::endl;
        return NULL;
    }
    gEDepSimTree->GetEntry(i);
    std::cout << "entry " << i;
    std::cout << " event " << gEDepSimEvent->EventId;
    std::cout << std::endl;
    return gEDepSimEvent;
}    
