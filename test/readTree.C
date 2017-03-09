#include "EDepSimEvents/EDepSimEventsProjectHeaders.h"

#include <TFile.h>
#include <TTree.h>

#include <iostream>

void readTree() {
    TTree* eventTree = (TTree*) gFile->Get("EDepSimEvents");
    if (!eventTree) {
        std::cout << "Missing the event tree" << std::endl;
        return;
    }
    
    eventTree->Print();
    eventTree->Scan();

    TG4Event *event = NULL;
    eventTree->SetBranchAddress("Event",&event);
    
    int entries = eventTree->GetEntries();
    std::cout << "Entries " << entries << std::endl;
    for (int i = 0; i<entries; ++i) {
        eventTree->GetEntry(i);
        std::cout << "entry " << i;
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
        typedef
            std::vector<std::pair<std::string,std::vector<TG4HitSegment> > >
            SegmentDetectorContainer;
        for (SegmentDetectorContainer::iterator
                 d = event->SegmentDetectors.begin();
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
}
