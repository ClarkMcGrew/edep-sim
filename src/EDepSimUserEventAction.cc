#include "EDepSimUserEventAction.hh"
#include "EDepSimException.hh"
#include "EDepSimUserEventInformation.hh"
#include "EDepSimVertexInfo.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimHitSegment.hh"

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

void EDepSim::UserEventAction::BeginOfEventAction(const G4Event* theEvent) {
    EDepSimNamedLog("Event", "Begin Event: " << theEvent->GetEventID()
                 << " w/ " << theEvent->GetNumberOfPrimaryVertex()
                 << " vertices");

    if (!theEvent->GetUserInformation()) {
        G4EventManager::GetEventManager()->
            SetUserInformation(new EDepSim::UserEventInformation);
    }

    int vtxNumber=0;
    for (G4PrimaryVertex* vtx = theEvent->GetPrimaryVertex();
         vtx;
         vtx = vtx->GetNext()) {
        ++vtxNumber;
        EDepSimNamedInfo(
            "Event",
            "Vertex: " << vtxNumber
            << " w/ " << vtx->GetNumberOfParticle() << " primaries");
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

    // Run the external actions.  These must not change the state of G4
    // or EDepSim.
    for (G4UserEventAction *action : fExternalActions) {
        action->BeginOfEventAction(theEvent);
    }

}

void EDepSim::UserEventAction::EndOfEventAction(const G4Event* theEvent) {
    EDepSimInfo("Event " << theEvent->GetEventID() << " completed.");

    // Notice that an event may legitimately have no hit collections
    // (e.g. when all of the energy is carried by optical photons handled
    // outside of GEANT4). Explicitly check first to minimize corner
    // cases in later code.
    G4HCofThisEvent* HCofEvent = theEvent->GetHCofThisEvent();
    if (HCofEvent == nullptr) {
        // There isn't a hit collection for this event, so that means nothing
        // has depositied energy into the hit segment sensitive detectors.
        // The external end of event actions still need to be notified so that
        // they see the trajectory information.  The actions will need to
        // distringuish between events with HCofEvent and events without
        // HCofEvent (i.e. events with and without hit segments).
        for (G4UserEventAction *action : fExternalActions) {
            action->EndOfEventAction(theEvent);
        }
        return;
    }

    G4SDManager *sdM = G4SDManager::GetSDMpointer();
    if (sdM == nullptr) {
        EDepSimThrow("Missing sensitive detector pointer");
    }

    G4HCtable *hcT = sdM->GetHCtable();
    if (hcT == nullptr) {
        EDepSimThrow("Missing hit collection table pointer");
    }

    // Fill the trajectories with the amount of energy deposited into
    // sensitive detectors.  This must happen before the external actions
    // are run so that they see the completed trajectory information.
    for (int i=0; i<hcT->entries(); ++i) {
        G4String SDname = hcT->GetSDname(i);
        G4String HCname = hcT->GetHCname(i);
        int HCId = sdM->GetCollectionID(SDname+"/"+HCname);
        G4VHitsCollection* g4Hits = HCofEvent->GetHC(HCId);

        // A sensitive detector can be registered in the hit collection
        // table without a collection existing for this event.  That
        // happens for a pseudo-SD that is not attached to any logical
        // volume (e.g. a detector filled from a GPU offloaded simulation
        // during the end of event action), so GetHC() returns null.
        if (g4Hits == nullptr or g4Hits->GetSize()<1) {
            EDepSimWarn("No hits for " << SDname << "/" << HCname);
            continue;
        }
        for (std::size_t h=0; h<g4Hits->GetSize(); ++h) {
            EDepSim::HitSegment* g4Hit
                = dynamic_cast<EDepSim::HitSegment*>(g4Hits->GetHit(h));
            if (g4Hit == nullptr) continue;
            double energy = g4Hit->GetEnergyDeposit();
            int trackId = g4Hit->GetContributors().front();
            G4VTrajectory* g4Traj = EDepSim::TrajectoryMap::Get(trackId);
            if (g4Traj == nullptr) {
                EDepSimError("Missing trackId " << trackId);
                continue;
            }
            EDepSim::Trajectory* traj
                = dynamic_cast<EDepSim::Trajectory*>(g4Traj);
            if (traj == nullptr) {
                EDepSimError("Not a EDepSim::Trajectory  " << trackId);
                continue;
            }
            traj->AddSDEnergyDeposit(energy);
            traj->AddSDLength(g4Hit->GetTrackLength());
            for (int loopCount = 0; ; ++loopCount) {
                int parentId = traj->GetParentID();
                // The parent is greater than zero when the parent exists.
                if (parentId <= 0) break;
                g4Traj = EDepSim::TrajectoryMap::Get(parentId);
                if (!g4Traj) {
                    EDepSimError("Missing parentId " << parentId);
                    break;
                }
                traj = dynamic_cast<EDepSim::Trajectory*>(g4Traj);
                if (traj == nullptr) {
                    EDepSimError("Not a EDepSim::Trajectory  " << trackId);
                    break;
                }
                traj->AddSDDaughterEnergyDeposit(energy);
                if (loopCount>9999) {
                    EDepSimError("Infinite loop for trajectory id "
                                 << trackId);
                    EDepSimThrow("Infinite loop trap");
                }
            }
        }
    }

    // Run the external actions.  These are run last so that they see the
    // final trajectory and hit segment information, and they are run for
    // every event, including events without any hit segments.  These must
    // not change the state of G4 or EDepSim.
    for (G4UserEventAction *action : fExternalActions) {
        action->EndOfEventAction(theEvent);
    }

}
