////////////////////////////////////////////////////////////
// $Id: DSimRootPersistencyManager.cc,v 1.93 2012/03/23 14:09:24 mcgrew Exp $
//

#include "DSimRootPersistencyManager.hh"
#include "DSimException.hh"
#include "DSimUserRunAction.hh"
#include "DSimUserPrimaryGeneratorAction.hh"
#include "DSimUserEventInformation.hh"
#include "DSimRootGeometryManager.hh"
#include "DSimTrajectory.hh"
#include "DSimTrajectoryPoint.hh"
#include "DSimHitSegment.hh"
#include "DSimVertexInfo.hh"
#include "DSimMagneticField.hh"
#include "kinem/DSimPrimaryGenerator.hh"
#include "kinem/DSimVCountGenerator.hh"

#include <globals.hh>

#include <G4Event.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <G4VisAttributes.hh>
#include <G4VSolid.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4Polyhedra.hh>
#include <G4Trap.hh>
#include <G4StepStatus.hh>
#include <G4TransportationManager.hh>
#include <G4FieldManager.hh>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoMedium.h>
#include <TGeoPgon.h>

#include <TCaptLog.hxx>
#include <TEvent.hxx>
#include <TMCHeader.hxx>
#include <TGeomIdManager.hxx>
#include <TManager.hxx>

#include <memory>
#include <cmath>
#include <algorithm>

TROOT root("ROOT","Root of Everything");

DSimRootPersistencyManager::DSimRootPersistencyManager() 
    : DSimPersistencyManager(), fOutput(NULL),
      fEventTree(NULL), fEvent(NULL), fEventsNotSaved(0) {}

DSimRootPersistencyManager* DSimRootPersistencyManager::GetInstance() {
    DSimRootPersistencyManager *current 
        = dynamic_cast<DSimRootPersistencyManager*>(
            G4VPersistencyManager::GetPersistencyManager());
    if (!current) current = new DSimRootPersistencyManager();
    return current;
}

DSimRootPersistencyManager::~DSimRootPersistencyManager() {
    if (fOutput) delete fOutput; fOutput = NULL;
}

bool DSimRootPersistencyManager::IsOpen() {
    if (fOutput && fOutput->IsOpen()) {
        fOutput->cd();
        return true;
    }
    return false;
};

bool DSimRootPersistencyManager::Open(G4String filename) {
    if (fOutput) {
        CaptLog("DSimRootPersistencyManager::Open "
                 << "-- Delete current file pointer" );
    }

    SetFilename(filename +".root");

    CaptLog("DSimRootPersistencyManager::Open " << GetFilename());

    fOutput = TFile::Open(GetFilename(), "RECREATE", "T2K-DSim Root Output");

    fEventTree = new TTree("captainEventTree","Tree of CAPTAIN Events");
    fEventTree->Branch("Event","CP::TEvent",&fEvent,128000,0);

    fEventsNotSaved = 0;

    return true;
}

bool DSimRootPersistencyManager::Close() {
    if (!fOutput) {
        CaptError("DSimRootPersistencyManager::Close "
                   << "-- No Output File");
        return false;
    }

    fOutput->cd();
    fOutput->Write();
    fOutput->Close();
    CaptLog("Output file " << GetFilename() << " closed.");

    fEventTree = NULL;
    fEvent = NULL;

    return true;
}

namespace {
    bool lt_prim_vertex(const CP::TG4PrimaryVertex& a,
                        const CP::TG4PrimaryVertex& b) {
        return a.GetPosition().T()<b.GetPosition().T();
    }
}

bool DSimRootPersistencyManager::Store(const G4Event* anEvent) {
    if (!fOutput) {
        CaptError("DSimRootPersistencyManager::Store "
                   << "-- No Output File");
        return false;
    }

    const G4Run* runInfo = G4RunManager::GetRunManager()->GetCurrentRun();
    const DSimUserRunAction* runAction 
        = dynamic_cast<const DSimUserRunAction*>(
            G4RunManager::GetRunManager()->GetUserRunAction());

    DSimUserEventInformation* eventInfo 
        = dynamic_cast<DSimUserEventInformation*>(
            anEvent->GetUserInformation());
    if (!eventInfo) {
        CaptError("  %%% Event without user information.");
        DSimError("Trying to write an event without user information");
    }

    int runId = runInfo->GetRunID();
    int subId = runAction->GetDetSimSubrunId();
    int eventId = anEvent->GetEventID();
    if (subId>0) eventId += 100000*subId;

    // Build the event context.
    CP::TEventContext context;
    context.SetRun(runId);
    if (subId >= 0) context.SetSubRun(subId);
    context.SetEvent(eventId);
    context.SetPartition(CP::TEventContext::kMCData);

    // The event is constructed using an auto ptr since we must delete it
    // before leaving this method.
    std::auto_ptr<CP::TEvent> event(new CP::TEvent(context));

    if (!event->FindDatum("truth")) {
        event->push_back(new CP::TDataVector("truth"));
    }

    // Mark the trajectories to save.
    MarkTrajectories(anEvent);

    // Save the trajectories.
    CopyTrajectories(*event,anEvent->GetTrajectoryContainer());

    // Save the primary particles.
    CP::TG4PrimaryVertexContainer* vertexContainer
        = MakePrimary("G4PrimVertex00",
                      "G4 Primary Particle Vertex",
                      anEvent->GetPrimaryVertex());
    if (vertexContainer) {
        // Sort the vertex container by time.
        std::stable_sort(vertexContainer->begin(), vertexContainer->end(), 
                         lt_prim_vertex);
        CP::THandle<CP::TDataVector> vect
            = event->Get<CP::TDataVector>("truth");
        vect->push_back(vertexContainer);
    }
    else {
        CaptVerbose("%% No Primary Particles to save");
    }
    
    // Save the informational particles.
    if (eventInfo) {
        // No info to be saved (be sure and change this comment if some is
        // added.
    }
    
    // Save all of the hits saved in this event.
    CopyHitCollections(*event,anEvent);

    // Save the MC Header.
    MakeMCHeader(*event, anEvent);

    fOutput->cd();
        
    // Get the address of the event from the auto_ptr to the fEventTree branch
    // pointer,, and  then fill the event  tree.  This copies the  data to the
    // output  file.  After the  data is  saved, I  reset the  the fEvent
    // pointer (the fEventTree branch pointer).
    fEvent = event.get();
    fEventTree->Fill();
    fEvent = NULL;

    ++fEventsNotSaved;
    if (fEventsNotSaved>100) {
        CaptInfo("AutoSave Event Tree");
        fEventTree->AutoSave("SaveSelf");
        fEventsNotSaved = 0;
    }

    return true;
}

bool DSimRootPersistencyManager::Store(const G4Run* aRun) {
    return false;
}

bool DSimRootPersistencyManager::Store(const G4VPhysicalVolume* aWorld) {
    if (!gGeoManager) {
        CaptError("DSimRootPersistencyManage::Store(world) run before /dsim/update");
        DSimRootGeometryManager::Get()->Update(aWorld,true);
    }
    if (!fOutput) {
        CaptError("DSimRootPersistencyManager::Store "
                   << "-- No Output File");
        return false;
    }
    fOutput->cd();
    gGeoManager->Write();
    return true;
}

std::vector<int>::iterator DSimRootPersistencyManager::CleanHitContributors(
    std::map<int,int>& parentMap,
    const CP::TG4TrajectoryContainer& trajCon,
    std::vector<int>::iterator start, 
    std::vector<int>::iterator finish) {

    for (std::vector<int>::iterator c=start; c != finish; ++c) {
        // Check each contributor to make sure that it is a valid
        // trajectory.  If it isn't in the trajectory map, then set it
        // to a parent that is.
        int loopTrap = 100000;
        while (!trajCon.GetTrajectory(*c)) {
            std::map<int,int>::iterator t = parentMap.find(*c);
            if (t == parentMap.end()) {
                CaptWarn("% Contributing trajectory without parent");
                break;
            }
            *c = t->second;
            if ( 0 > --loopTrap) {
                CaptWarn("Break loop in "
                          << "DSimRootPersistencyManager::"
                          << "CleanHitContributors");
                break;
            }
        }
    }

    // Remove the duplicate entries.
    std::sort(start,finish);
    finish = std::unique(start,finish);
        
    return finish;
}

void DSimRootPersistencyManager::FillParentMap(std::map<int,int>& parentMap,
                                                    const G4Event* src) {
    parentMap.clear();

    const G4TrajectoryContainer* g4Traj = src->GetTrajectoryContainer();
    if (g4Traj) {
        for (TrajectoryVector::iterator t = g4Traj->GetVector()->begin();
             t != g4Traj->GetVector()->end();
             ++t) {
            parentMap[(*t)->GetTrackID()] = (*t)->GetParentID();
        }
    }
    else {
        CaptWarn("%%% Missing trajectories for Hit Contributors");
    }
}
                                                       
CP::TG4HitContainer* DSimRootPersistencyManager::CreateHitSegmentContainer(
    std::map<int,int>& parentMap,
    const CP::TG4TrajectoryContainer& trajCon,
    G4VHitsCollection* g4Hits,
    std::string hitsName,
    CP::TEvent& dest) {

    if (!g4Hits) return NULL;
    if (g4Hits->GetSize()<1) return NULL;
    // Check to make sure that this hit collection contains DSimHitSegment
    // objects.
    DSimHitSegment* g4Hit = dynamic_cast<DSimHitSegment*>(g4Hits->GetHit(0));
    if (!g4Hit) return NULL;

    CP::TG4HitContainer* hits = new CP::TG4HitContainer(hitsName.c_str());
    for (unsigned int h=0; h<g4Hits->GetSize(); ++h) {
        DSimHitSegment* g4Hit
            = dynamic_cast<DSimHitSegment*>(g4Hits->GetHit(h));
        std::vector<int>::iterator e = 
            CleanHitContributors(parentMap,trajCon,
                                 g4Hit->GetContributors().begin(),
                                 g4Hit->GetContributors().end());
        g4Hit->GetContributors().erase(e,g4Hit->GetContributors().end());
        CP::TG4HitSegment* hit = dynamic_cast<CP::TG4HitSegment*>(g4Hit);
        hits->push_back(new CP::TG4HitSegment(*hit));
    }
    
    return hits;
}
                                                       
void DSimRootPersistencyManager::CopyHitCollections(CP::TEvent& dest, 
                                                     const G4Event* src) {
    G4HCofThisEvent* HCofEvent = src->GetHCofThisEvent();
    G4SDManager *sdM = G4SDManager::GetSDMpointer();
    G4HCtable *hcT = sdM->GetHCtable();
    if (!HCofEvent) return;

    // Make sure that the directory for G4 hits exists in the output event.
    if (!dest.FindDatum("truth/g4Hits")) {
        if (!dest.FindDatum("truth")) {
            dest.push_back(new CP::TDataVector("truth"));
        }
        dest.Get<CP::TDataVector>("truth")->push_back(
            new CP::TDataVector("g4Hits"));
    }

    //Keep track of all trajectories and which ones are saved.
    std::map<int,int> parentMap;
    FillParentMap(parentMap,src);

    // Get the trajectories that have been saved in the event.
    CP::THandle<CP::TG4TrajectoryContainer> trajCon 
        = dest.Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");

    // Copy each of the hit categories into the output event.
    for (int i=0; i<hcT->entries(); ++i) {
        G4String SDname = hcT->GetSDname(i);
        G4String HCname = hcT->GetHCname(i);
        int HCId = sdM->GetCollectionID(SDname+"/"+HCname);
        G4VHitsCollection* g4Hits = HCofEvent->GetHC(HCId);
        if (g4Hits->GetSize()<1) continue;
        std::string hitsName = SDname;
        CP::TDatum* hits = NULL;
        if (!hits) hits = CreateHitSegmentContainer(parentMap,*trajCon,
                                                    g4Hits, hitsName, dest);
        if (hits) dest.Get<CP::TDataVector>("truth/g4Hits")->push_back(hits);
    }
}

CP::TG4PrimaryVertexContainer* DSimRootPersistencyManager::MakePrimary(
    const char* name, const char* title, const G4PrimaryVertex* src) {
    if (!src) return NULL;

    CP::TG4PrimaryVertexContainer* vertexContainer = NULL;
    while (src) {
        CP::TG4PrimaryVertex vtx;
        
        vtx.GetPosition().SetX(src->GetX0());
        vtx.GetPosition().SetY(src->GetY0());
        vtx.GetPosition().SetZ(src->GetZ0());
        vtx.GetPosition().SetT(src->GetT0());
        
        for (int i=0; i< src->GetNumberOfParticle(); ++i) {
            CP::TG4PrimaryParticle prim;
            G4PrimaryParticle *g4Prim = src->GetPrimary(i);
            prim.SetTrackId(g4Prim->GetTrackID());
            prim.SetPDGCode(g4Prim->GetPDGcode());
            prim.SetTitle(g4Prim->GetG4code()->GetParticleName());
            prim.GetMomentum().SetPx(g4Prim->GetPx());
            prim.GetMomentum().SetPy(g4Prim->GetPy());
            prim.GetMomentum().SetPz(g4Prim->GetPz());
            double E = pow(prim.GetMomentum().P(),2) 
                + pow(g4Prim->GetG4code()->GetPDGMass(),2);
            if (E>0) E = sqrt(E);
            else E = 0;
            prim.GetMomentum().SetE(E);
            vtx.GetPrimaryParticles().push_back(prim);
            CaptVerbose("   "
                      << " id " << g4Prim->GetTrackID()
                      << " " << g4Prim->GetG4code()->GetParticleName()
                      << " with " << G4BestUnit(prim.GetMomentum().E(),"Energy")
                      << " (from " << G4BestUnit(g4Prim->GetMomentum().mag(), 
                                                 "Energy") << "/c" 
                      << " + " << G4BestUnit(prim.GetMomentum().M(), 
                                             "Energy") << "/c^2"
                      << ")");
        }

        DSimVertexInfo* srcInfo 
            = dynamic_cast<DSimVertexInfo*>(src->GetUserInformation());
        if (srcInfo) {
            vtx.SetGeneratorName(srcInfo->GetName());
            vtx.SetReaction(srcInfo->GetReaction());
            vtx.SetFilename(srcInfo->GetFilename());
            vtx.SetInteractionNumber(srcInfo->GetInteractionNumber());
            vtx.SetCrossSection(srcInfo->GetCrossSection());
            vtx.SetDiffCrossSection(srcInfo->GetDiffCrossSection());
            vtx.SetWeight(srcInfo->GetWeight());
            vtx.SetProbability(srcInfo->GetProbability());
            CP::TG4PrimaryVertexContainer *vtxInfo
                = MakePrimary("Informational","Informational Vertex",
                              srcInfo->GetInformationalVertex());
            if (vtxInfo) {
                vtx.SetInfoVertex(*vtxInfo);
                delete vtxInfo;
            }
        }

        if (!vertexContainer) {
            vertexContainer = new CP::TG4PrimaryVertexContainer(name,title);
        }
        vertexContainer->push_back(vtx);
        src = src->GetNext();
    }
    
    return vertexContainer;
}

void DSimRootPersistencyManager::CopyTrajectoryPoints(CP::TG4Trajectory& traj, 
                                                       G4VTrajectory* g4Traj) {
    std::vector<int> selected;

    // Choose the trajectory points that are going to be saved.
    SelectTrajectoryPoints(selected, g4Traj);

    // Make sure the selected trajectory points are in order and unique.
    std::sort(selected.begin(),selected.end());
    selected.erase(std::unique(selected.begin(), selected.end()),
                   selected.end());

    ////////////////////////////////////
    // Save the trajectories.
    ////////////////////////////////////
    for (std::vector<int>::iterator tp = selected.begin(); 
         tp != selected.end(); ++tp) {
        DSimTrajectoryPoint* dsimPoint 
            = dynamic_cast<DSimTrajectoryPoint*>(g4Traj->GetPoint(*tp));
        CP::TG4TrajectoryPoint point;
        point.SetPosition(dsimPoint->GetPosition().x(),
                          dsimPoint->GetPosition().y(),
                          dsimPoint->GetPosition().z(),
                          dsimPoint->GetTime());
        point.SetMomentum(dsimPoint->GetMomentum().x(),
                          dsimPoint->GetMomentum().y(),
                          dsimPoint->GetMomentum().z());
        point.SetVolumeNode(dsimPoint->GetVolumeNode());
        traj.GetTrajectoryPoints().push_back(point);
    }

}

void DSimRootPersistencyManager::CopyTrajectories(
    CP::TEvent& dest,
    const G4TrajectoryContainer* src) {
    if (!src) {
        CaptVerbose("No Trajectories ");
        return;
    }

    CP::TG4TrajectoryContainer* traj = new CP::TG4TrajectoryContainer();

    // Keep track of all trajectories and which ones are saved.  The first
    // field is the track id, and the second is whether it is saved.
    std::map<int,std::pair<int,bool> > parentMap;
    for (TrajectoryVector::iterator t = src->GetVector()->begin();
         t != src->GetVector()->end();
         ++t) {
        DSimTrajectory* ndTraj = dynamic_cast<DSimTrajectory*>(*t);

        parentMap[ndTraj->GetTrackID()] 
            = std::pair<int,bool>(ndTraj->GetParentID(),false);

        // Check if the trajectory should be saved.
        if (!ndTraj->SaveTrajectory()) continue;
        parentMap[(*t)->GetTrackID()].second = true;

        std::string particleName = ndTraj->GetParticleName();

        CP::TG4Trajectory copy;
        copy.SetTrackId((*t)->GetTrackID());
        // Set the particle type information.
        G4ParticleDefinition* part 
            = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
        if (!part) {
            DSimError(std::string("DSimRootPersistencyManager::")
                        + "No particle information for "
                        + (*t)->GetParticleName());
        }
        copy.SetParticleName(particleName);
        copy.SetPDGEncoding((*t)->GetPDGEncoding());
        // Check through the list of saved trajectories to find the first
        // parent that is written to the output file.
        int parentId = (*t)->GetTrackID();
        do {
            parentId = parentMap[parentId].first;
        } while (parentId != 0 && !parentMap[parentId].second);
        copy.SetParentId(parentId);
        // Set the particle momentum.
        TLorentzVector mom;
        mom.SetXYZM((*t)->GetInitialMomentum().x(),
                    (*t)->GetInitialMomentum().y(),
                    (*t)->GetInitialMomentum().z(),
                    part->GetPDGMass());
        copy.SetInitialMomentum(mom);
        CopyTrajectoryPoints(copy, (*t));
        (*traj)[copy.GetTrackId()] = copy;
    }

    CP::THandle<CP::TDataVector> vect = dest.Get<CP::TDataVector>("truth");
    vect->push_back(traj);
}

void DSimRootPersistencyManager::MakeMCHeader(CP::TEvent& dest,
                                              const G4Event* src) {
    // create the MC Header.
    CP::TMCHeader* mcHeader = new CP::TMCHeader();
    CP::THandle<CP::TDataVector> vect = dest.Get<CP::TDataVector>("truth");
    vect->AddDatum(mcHeader);

    // Save the hash code of the geometry for this event.
    mcHeader->SetGeometryHash(CP::TManager::Get().GeomId().GetHash());

    const G4RunManager* runMan = G4RunManager::GetRunManager();
    const DSimUserPrimaryGeneratorAction* userGenerator
        = dynamic_cast<const DSimUserPrimaryGeneratorAction*>(
            runMan->GetUserPrimaryGeneratorAction());
    if (!userGenerator) DSimError("Missing DSimUserPrimaryGeneratorAction");
    const DSimPrimaryGenerator* primGenerator 
        = dynamic_cast<const DSimPrimaryGenerator*>(
            userGenerator->GetGenerator(0));
    if (!primGenerator) DSimError("Missing DSimUserPrimaryGeneratorAction");
    double intensity = primGenerator->GetCountGenerator()->GetIntensity();

    mcHeader->SetIntensity(intensity);
}
