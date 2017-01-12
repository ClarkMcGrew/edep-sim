#include <TGeoManager.h>
#include <TGeoNode.h>

#include "EDepSimUserEventAction.hh"
#include "EDepSimException.hh"
#include "EDepSimUserEventInformation.hh"
#include "EDepSimVertexInfo.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimHitSegment.hh"
#include "EDepSimRootGeometryManager.hh"

#include "EDepSimLog.hh"

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

EDepSim::UserEventAction::UserEventAction() {}

EDepSim::UserEventAction::~UserEventAction() {}

void EDepSim::UserEventAction::BeginOfEventAction(const G4Event* evt) {
    EDepSimNamedLog("Event", "Begin Event: " << evt->GetEventID() 
                 << " w/ " << evt->GetNumberOfPrimaryVertex()
                 << " vertices");
    
    // The last chance to create the user information object.  This should be
    // created in the primary particle generater
    if (!evt->GetUserInformation()) {
        G4EventManager::GetEventManager()->
            SetUserInformation(new EDepSim::UserEventInformation);
    }

    int vtxNumber=0;
    for (G4PrimaryVertex* vtx = evt->GetPrimaryVertex();
         vtx; 
         vtx = vtx->GetNext()) {
        ++vtxNumber;
        gGeoManager->PushPath();
        EDepSim::RootGeometryManager::Get()->GetNodeId(
            G4ThreeVector(vtx->GetX0(), vtx->GetY0(), vtx->GetZ0()));
        EDepSimNamedInfo(
            "Event",
            "Vertex: " << vtxNumber  
            << " w/ " << vtx->GetNumberOfParticle() << " primaries"
            " in " << gGeoManager->GetPath());
        gGeoManager->PopPath();
        EDepSimNamedVerbose(
            "Event",
            "Position: " 
            << " (" << G4BestUnit(vtx->GetX0(),"Length") 
            << ", " << G4BestUnit(vtx->GetY0(),"Length") 
            << ", " << G4BestUnit(vtx->GetZ0(),"Length") 
            << ", " << G4BestUnit(vtx->GetT0(),"Time") << ")");
        EDepSim::VertexInfo* vInfo 
            = dynamic_cast<EDepSim::VertexInfo*>(vtx->GetUserInformation());
        if (vInfo) {
            EDepSimNamedInfo("Event","Generator: " << vInfo->GetName());
            EDepSimNamedInfo("Event","Reaction:  " << vInfo->GetReaction());
            int infoVertices = vInfo->GetNumberOfInformationalVertex();
            for (int iVert = 0; iVert<infoVertices; ++iVert) {
                const G4PrimaryVertex* ivtx 
                    = vInfo->GetInformationalVertex(iVert);
                for (int p=0; p<ivtx->GetNumberOfParticle(); ++p) {
                    G4PrimaryParticle* prim = ivtx->GetPrimary(p);
                    G4ParticleDefinition* partDef = prim->GetG4code();
                    G4ThreeVector dir = prim->GetMomentum().unit();
                    if (partDef) {
                        EDepSimNamedInfo(
                            "Event",
                            "Info: " << partDef->GetParticleName()
                            << " w/ "
                            << G4BestUnit(prim->GetMomentum().mag(),"Energy")
                            << "  Dir: (" << dir.x()
                            << ", " << dir.y()
                            << ", " << dir.z() << ")");
                    }
                    else {
                        EDepSimNamedInfo(
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
                EDepSimNamedVerbose(
                    "Event",
                    partDef->GetParticleName()
                    << " w/ "
                    << G4BestUnit(prim->GetMomentum().mag(),"Energy")
                    << "  Dir: (" << dir.x()
                    << ", " << dir.y()
                    << ", " << dir.z() << ")");
            }
            else {
                EDepSimNamedVerbose(
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

    EDepSim::TrajectoryMap::Clear();
}

void EDepSim::UserEventAction::EndOfEventAction(const G4Event* evt) {
    EDepSimInfo("Event " << evt->GetEventID() << " completed.");

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
            EDepSimError("No hits for " << SDname << "/" << HCname); 
            continue;
        }
        for (unsigned int h=0; h<g4Hits->GetSize(); ++h) {
            EDepSim::HitSegment* g4Hit
                = dynamic_cast<EDepSim::HitSegment*>(g4Hits->GetHit(h));
            double energy = g4Hit->GetEnergyDeposit();
            int trackId = g4Hit->GetContributors().front();
            G4VTrajectory* g4Traj = EDepSim::TrajectoryMap::Get(trackId);
            if (!g4Traj) {
                EDepSimError("Missing trackId " << trackId);
                continue;
            }
            EDepSim::Trajectory* traj = dynamic_cast<EDepSim::Trajectory*>(g4Traj);
            if (!traj) {
                EDepSimError("Not a EDepSim::Trajectory  " << trackId);
                continue;
            }
            traj->AddSDEnergyDeposit(energy);
            traj->AddSDLength(g4Hit->GetTrackLength());
            for (int loopCount = 0; ; ++loopCount) {
                int parentId = traj->GetParentID();
                if (!parentId) break;
                g4Traj = EDepSim::TrajectoryMap::Get(parentId);
                if (!g4Traj) {
                    EDepSimError("Missing parentId " << parentId);
                    break;
                }
                traj = dynamic_cast<EDepSim::Trajectory*>(g4Traj);
                if (!traj) {
                    EDepSimError("Not a EDepSim::Trajectory  " << trackId);
                    break;
                }
                traj->AddSDDaughterEnergyDeposit(energy);
                if (loopCount>9999) {
                    EDepSimError("Infinite loop for trajectory id " << trackId);
                    EDepSimThrow("Infinite loop trap");
                }
            }
        }
    }
}
