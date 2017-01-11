#include <TGeoManager.h>
#include <TGeoNode.h>

#include "DSimUserEventAction.hh"
#include "DSimException.hh"
#include "DSimUserEventInformation.hh"
#include "DSimVertexInfo.hh"
#include "DSimTrajectoryMap.hh"
#include "DSimTrajectory.hh"
#include "DSimHitSegment.hh"
#include "DSimRootGeometryManager.hh"

#include "DSimLog.hh"

#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4UnitsTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4ParticleDefinition.hh>
#include <G4ios.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <Randomize.hh>

DSimUserEventAction::DSimUserEventAction() {}

DSimUserEventAction::~DSimUserEventAction() {}

void DSimUserEventAction::BeginOfEventAction(const G4Event* evt) {
    DSimNamedLog("Event", "Begin Event: " << evt->GetEventID() 
                 << " w/ " << evt->GetNumberOfPrimaryVertex()
                 << " vertices");
    
    // The last chance to create the user information object.  This should be
    // created in the primary particle generater
    if (!evt->GetUserInformation()) {
        G4EventManager::GetEventManager()->
            SetUserInformation(new DSimUserEventInformation);
    }

    int vtxNumber=0;
    for (G4PrimaryVertex* vtx = evt->GetPrimaryVertex();
         vtx; 
         vtx = vtx->GetNext()) {
        ++vtxNumber;
        gGeoManager->PushPath();
        DSimRootGeometryManager::Get()->GetNodeId(
            G4ThreeVector(vtx->GetX0(), vtx->GetY0(), vtx->GetZ0()));
        DSimNamedInfo(
            "Event",
            "Vertex: " << vtxNumber  
            << " w/ " << vtx->GetNumberOfParticle() << " primaries"
            " in " << gGeoManager->GetPath());
        gGeoManager->PopPath();
        DSimNamedVerbose(
            "Event",
            "Position: " 
            << " (" << G4BestUnit(vtx->GetX0(),"Length") 
            << ", " << G4BestUnit(vtx->GetY0(),"Length") 
            << ", " << G4BestUnit(vtx->GetZ0(),"Length") 
            << ", " << G4BestUnit(vtx->GetT0(),"Time") << ")");
        DSimVertexInfo* vInfo 
            = dynamic_cast<DSimVertexInfo*>(vtx->GetUserInformation());
        if (vInfo) {
            DSimNamedInfo("Event","Generator: " << vInfo->GetName());
            DSimNamedInfo("Event","Reaction:  " << vInfo->GetReaction());
            int infoVertices = vInfo->GetNumberOfInformationalVertex();
            for (int iVert = 0; iVert<infoVertices; ++iVert) {
                const G4PrimaryVertex* ivtx 
                    = vInfo->GetInformationalVertex(iVert);
                for (int p=0; p<ivtx->GetNumberOfParticle(); ++p) {
                    G4PrimaryParticle* prim = ivtx->GetPrimary(p);
                    G4ParticleDefinition* partDef = prim->GetG4code();
                    G4ThreeVector dir = prim->GetMomentum().unit();
                    if (partDef) {
                        DSimNamedInfo(
                            "Event",
                            "Info: " << partDef->GetParticleName()
                            << " w/ "
                            << G4BestUnit(prim->GetMomentum().mag(),"Energy")
                            << "  Dir: (" << dir.x()
                            << ", " << dir.y()
                            << ", " << dir.z() << ")");
                    }
                    else {
                        DSimNamedInfo(
                            "Event",
                            "Info: " << prim->GetPDGcode()
                            << " w/ "
                            << G4BestUnit(prim->GetMomentum().mag(),"Energy")
                            << "  Dir: (" << dir.x()
                            << ", " << dir.y()
                            << ", " << dir.z() << ")");
                    }
                }
            }
        }
        for (int p=0; p<vtx->GetNumberOfParticle(); ++p) {
            G4PrimaryParticle* prim = vtx->GetPrimary(p);
            G4ParticleDefinition* partDef = prim->GetG4code();
            G4ThreeVector dir = prim->GetMomentum().unit();
            if (partDef) {
                DSimNamedVerbose(
                    "Event",
                    partDef->GetParticleName()
                    << " w/ "
                    << G4BestUnit(prim->GetMomentum().mag(),"Energy")
                    << "  Dir: (" << dir.x()
                    << ", " << dir.y()
                    << ", " << dir.z() << ")");
            }
            else {
                DSimNamedVerbose(
                    "Event",
                    prim->GetPDGcode()
                    << " w/ "
                    << G4BestUnit(prim->GetMomentum().mag(),"Energy")
                    << "  Dir: (" << dir.x()
                    << ", " << dir.y()
                    << ", " << dir.z() << ")");
            }
        }
    }

    DSimTrajectoryMap::Clear();
}

void DSimUserEventAction::EndOfEventAction(const G4Event* evt) {
    DSimInfo("Event " << evt->GetEventID() << " completed.");

    // Fill the trajectories with the amount of energy deposited into
    // sensitive detectors.
    G4HCofThisEvent* HCofEvent = evt->GetHCofThisEvent();
    if (!HCofEvent) return;
    G4SDManager *sdM = G4SDManager::GetSDMpointer();
    G4HCtable *hcT = sdM->GetHCtable();

    for (int i=0; i<hcT->entries(); ++i) {
        G4String SDname = hcT->GetSDname(i);
        G4String HCname = hcT->GetHCname(i);
        int HCId = sdM->GetCollectionID(SDname+"/"+HCname);
        G4VHitsCollection* g4Hits = HCofEvent->GetHC(HCId);
        if (g4Hits->GetSize()<1) {
            DSimError("No hits for " << SDname << "/" << HCname); 
            continue;
        }
        for (unsigned int h=0; h<g4Hits->GetSize(); ++h) {
            DSimHitSegment* g4Hit
                = dynamic_cast<DSimHitSegment*>(g4Hits->GetHit(h));
            double energy = g4Hit->GetEnergyDeposit();
            int trackId = g4Hit->GetContributors().front();
            G4VTrajectory* g4Traj = DSimTrajectoryMap::Get(trackId);
            if (!g4Traj) {
                DSimError("Missing trackId " << trackId);
                continue;
            }
            DSimTrajectory* traj = dynamic_cast<DSimTrajectory*>(g4Traj);
            if (!traj) {
                DSimError("Not a DSimTrajectory  " << trackId);
                continue;
            }
            traj->AddSDEnergyDeposit(energy);
            traj->AddSDLength(g4Hit->GetTrackLength());
            for (int loopCount = 0; ; ++loopCount) {
                int parentId = traj->GetParentID();
                if (!parentId) break;
                g4Traj = DSimTrajectoryMap::Get(parentId);
                if (!g4Traj) {
                    DSimError("Missing parentId " << parentId);
                    break;
                }
                traj = dynamic_cast<DSimTrajectory*>(g4Traj);
                if (!traj) {
                    DSimError("Not a DSimTrajectory  " << trackId);
                    break;
                }
                traj->AddSDDaughterEnergyDeposit(energy);
                if (loopCount>9999) {
                    DSimError("Infinite loop for trajectory id " << trackId);
                    DSimThrow("Infinite loop trap");
                }
            }
        }
    }
}
