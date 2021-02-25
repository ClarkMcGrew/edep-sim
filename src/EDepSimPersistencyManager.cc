//
// Implementation for concrete G4PersistencyManager.
//

#include <TPRegexp.h>

#include "EDepSimPersistencyManager.hh"
#include "EDepSimPersistencyMessenger.hh"
#include "EDepSimVertexInfo.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimTrajectoryPoint.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimHitSegment.hh"
#include "EDepSimException.hh"
#include "EDepSimUserRunAction.hh"
#include "EDepSimLog.hh"

#include <G4ios.hh>
#include <G4RunManager.hh>
#include <G4Event.hh>
#include <G4Run.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4StepStatus.hh>
#include <G4ProcessType.hh>
#include <G4EmProcessSubType.hh>
#include <G4UnitsTable.hh>
#include <G4ParticleTable.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <memory>

// Handle foraging the edep-sim information into convenience classes that are
// independent of geant4 and edep-sim internal dependencies.  The classes are
// suitable for saving directly to root (and will be by default if this class
// isn't inherited).  This class can be inherited, and the convenience objects
// can be accessed using specific getters.  If this class is used directly,
// then it will create a simple tree that can be analyzed using root.  The
// best way to access the tree is with the root interface to python (no
// headers needed), or by using ROOT TFile::MakeProject.
EDepSim::PersistencyManager::PersistencyManager()
    : G4VPersistencyManager(), fFilename("/dev/null"),
      fLengthThreshold(10*mm),
      fGammaThreshold(5*MeV), fNeutronThreshold(50*MeV),
      fTrajectoryPointAccuracy(1.*mm), fTrajectoryPointDeposit(0*MeV),
      fSaveAllPrimaryTrajectories(true) {
    fPersistencyMessenger = new EDepSim::PersistencyMessenger(this);
}

// This is called by the G4RunManager during AnalyzeEvent.
EDepSim::PersistencyManager::~PersistencyManager() {
    ClearTrajectoryBoundaries();
    delete fPersistencyMessenger;
}

G4bool EDepSim::PersistencyManager::Open(G4String filename) {
    EDepSimSevere(" -- Open is not implimented for " << filename);
    SetFilename(filename);
    return false;
}

/// Make sure the output file is closed.
G4bool EDepSim::PersistencyManager::Close(void) {
    EDepSimSevere(" -- Close is not implimented.");
    return false;
}

// This is called by the G4RunManager during AnalyzeEvent.
G4bool EDepSim::PersistencyManager::Store(const G4Event* anEvent) {
    UpdateSummaries(anEvent);
    return false;
}

// This is called by the G4RunManager during AnalyzeEvent.
G4bool EDepSim::PersistencyManager::Store(const G4Run* aRun) {
    if (!aRun) return false;
    EDepSimSevere(" -- Run store called without a save method for "
               << "GEANT4 run " << aRun->GetRunID());
    return false;
}

// This is called by the G4RunManager during AnalyzeEvent.
G4bool EDepSim::PersistencyManager::Store(const G4VPhysicalVolume* aWorld) {
    if (!aWorld) return false;
    EDepSimSevere(" -- Geometry store called without a save method for "
               << aWorld->GetName());
    return false;
}

void EDepSim::PersistencyManager::AddTrajectoryBoundary(const G4String& b) {
    TPRegexp* bound = new TPRegexp(b.c_str());
    fTrajectoryBoundaries.push_back(bound);
}

void EDepSim::PersistencyManager::ClearTrajectoryBoundaries() {
    for (std::vector<TPRegexp*>::iterator r = fTrajectoryBoundaries.begin();
         r != fTrajectoryBoundaries.end();
         ++r) {
        delete (*r);
    }
    fTrajectoryBoundaries.clear();
}

bool EDepSim::PersistencyManager::SaveTrajectoryBoundary(G4VTrajectory* g4Traj,
                                                    G4StepStatus status,
                                                    G4String currentVolume,
                                                    G4String prevVolume) {
    if (status != fGeomBoundary) return false;
    std::string particleInfo = ":" + g4Traj->GetParticleName();
    if (std::abs(g4Traj->GetCharge())<0.1) particleInfo += ":neutral";
    else particleInfo += ":charged";
    std::string current = particleInfo + ":" + currentVolume;
    std::string previous = particleInfo + ":" + prevVolume;
    for (std::vector<TPRegexp*>::iterator r = fTrajectoryBoundaries.begin();
         r != fTrajectoryBoundaries.end();
         ++r) {
        // Check if a watched volume is being entered.
        if ((*r)->Match(current)>0 && (*r)->Match(previous)<1) {
            EDepSimNamedDebug("boundary","Entering " << current);
            return true;
        }
        // Check if a watched volume is being exited.
        if ((*r)->Match(current)<1 && (*r)->Match(previous)>0) {
            EDepSimNamedDebug("boundary","Exiting " << current);
            return true;
        }
    }
    return false;
}

void EDepSim::PersistencyManager::UpdateSummaries(const G4Event* event) {

    const G4Run* runInfo = G4RunManager::GetRunManager()->GetCurrentRun();

    fEventSummary.RunId = runInfo->GetRunID();
    fEventSummary.EventId = event->GetEventID();
    EDepSimLog("Event Summary for run " << fEventSummary.RunId
               << " event " << fEventSummary.EventId);

    // Summarize the trajectories first so that fTrackIdMap is filled.
    MarkTrajectories(event);

    SummarizePrimaries(fEventSummary.Primaries,event->GetPrimaryVertex());
    EDepSimLog("   Primaries " << fEventSummary.Primaries.size());

    SummarizeTrajectories(fEventSummary.Trajectories,event);
    EDepSimLog("   Trajectories " << fEventSummary.Trajectories.size());

    SummarizeSegmentDetectors(fEventSummary.SegmentDetectors, event);
    EDepSimLog("   Segment Detectors "
               << fEventSummary.SegmentDetectors.size());
}

void EDepSim::PersistencyManager::SummarizePrimaries(
    std::vector<TG4PrimaryVertex>& dest,
    const G4PrimaryVertex* src) {
    dest.clear();

    if (!src) return;

    while (src) {
        TG4PrimaryVertex vtx;

        vtx.Position.SetX(src->GetX0());
        vtx.Position.SetY(src->GetY0());
        vtx.Position.SetZ(src->GetZ0());
        vtx.Position.SetT(src->GetT0());

        // Add the particles associated with the vertex to the summary.
        for (int i=0; i< src->GetNumberOfParticle(); ++i) {
            TG4PrimaryParticle prim;
            G4PrimaryParticle *g4Prim = src->GetPrimary(i);
            double E = 0.0;
            if (g4Prim->GetG4code()) {
                prim.Name = g4Prim->GetG4code()->GetParticleName();
                E = pow(g4Prim->GetG4code()->GetPDGMass(),2);
            }
            else {
                prim.Name = "unknown";
            }
            prim.PDGCode = g4Prim->GetPDGcode();
            prim.TrackId = g4Prim->GetTrackID() - 1;
            prim.Momentum.SetX(g4Prim->GetPx());
            prim.Momentum.SetY(g4Prim->GetPy());
            prim.Momentum.SetZ(g4Prim->GetPz());
            E += pow(prim.Momentum.P(),2);
            if (E>0) E = std::sqrt(E);
            else E = 0;
            prim.Momentum.SetE(E);
            vtx.Particles.push_back(prim);
        }

        // Check to see if there is anyu user information associated with the
        // vertex.
        EDepSim::VertexInfo* srcInfo
            = dynamic_cast<EDepSim::VertexInfo*>(src->GetUserInformation());
        if (srcInfo) {
            vtx.GeneratorName = srcInfo->GetName();
            vtx.Reaction = srcInfo->GetReaction();
            vtx.Filename = srcInfo->GetFilename();
            vtx.InteractionNumber = srcInfo->GetInteractionNumber();
            vtx.CrossSection = srcInfo->GetCrossSection();
            vtx.DiffCrossSection = srcInfo->GetDiffCrossSection();
            vtx.Weight = srcInfo->GetWeight();
            vtx.Probability = srcInfo->GetProbability();

            const G4PrimaryVertex* infoVertex
                = srcInfo->GetInformationalVertex();
            if (infoVertex) {
                SummarizePrimaries(vtx.Informational,
                                   srcInfo->GetInformationalVertex());
            }
        }

        dest.push_back(vtx);
        src = src->GetNext();
    }
}

void EDepSim::PersistencyManager::SummarizeTrajectories(
    TG4TrajectoryContainer& dest,
    const G4Event* event) {
    dest.clear();
    MarkTrajectories(event);

    // Build a map of the original G4 TrackID to the new relocated TrackId
    // (not capitalization).  This also uses the fact that maps are sorted as
    // a hack to prevent writing a predicate to sort TG4Trajectories (because
    // I'm lazy).
    fTrackIdMap.clear();

    const G4TrajectoryContainer* trajectories = event->GetTrajectoryContainer();
    if (!trajectories) {
        EDepSimError("No trajectory container");
        return;
    }
    if (!trajectories->GetVector()) {
        EDepSimError("No trajectory vector in trajectory container");
        return;
    }

    int index = 0;
    TG4TrajectoryContainer tempContainer;
    for (TrajectoryVector::iterator t = trajectories->GetVector()->begin();
         t != trajectories->GetVector()->end();
         ++t) {
        EDepSim::Trajectory* ndTraj = dynamic_cast<EDepSim::Trajectory*>(*t);

        // Check if the trajectory should be saved.
        if (!ndTraj->SaveTrajectory()) continue;

        // Set the particle type information.
        G4ParticleDefinition* part
            = G4ParticleTable::GetParticleTable()->FindParticle(
                ndTraj->GetParticleName());
        if (!part) {
            EDepSimError(std::string("EDepSim::RootPersistencyManager::")
                      + "No particle information for "
                      + ndTraj->GetParticleName());
        }

        fTrackIdMap[ndTraj->GetTrackID()] = index++;

        TG4Trajectory traj;
        traj.TrackId = ndTraj->GetTrackID();
        traj.Name = ndTraj->GetParticleName();
        traj.PDGCode = ndTraj->GetPDGEncoding();
        traj.ParentId = ndTraj->GetParentID();
        traj.InitialMomentum.SetXYZM(ndTraj->GetInitialMomentum().x(),
                                     ndTraj->GetInitialMomentum().y(),
                                     ndTraj->GetInitialMomentum().z(),
                                     part->GetPDGMass());
        CopyTrajectoryPoints(traj,ndTraj);
        int throttle = 999999;
        do {
            if (traj.ParentId == 0) break;
            EDepSim::Trajectory* pTraj
                = dynamic_cast<EDepSim::Trajectory*>(
                    EDepSim::TrajectoryMap::Get(traj.ParentId));
            if (!pTraj) {
                EDepSimError("Trajectory " << traj.ParentId << " does not exist");
                throw;
                break;
            }
            if (pTraj->SaveTrajectory()) break;
            traj.ParentId = pTraj->GetParentID();
        } while (--throttle > 0);
        tempContainer.push_back(traj);
    }

    // Reorder the trajectories using the sorted fTrackIdMap.  This uses the
    // fact that the map is going to have the right sort order.
    for (TrackIdMap::iterator i = fTrackIdMap.begin();
         i != fTrackIdMap.end(); ++i) {
        dest.push_back(tempContainer[i->second]);
    }

    // Now that the trajectories are reordered, reassign the mapping from the
    // original ndTraj->GetTrackID() value to the index in the Trajectories
    // vector.
    index = 0;
    for (TrackIdMap::iterator i = fTrackIdMap.begin();
         i != fTrackIdMap.end(); ++i) {
        i->second = index++;
    }

    // Make double sure that TrackID zero maps to the non-existent -1.
    fTrackIdMap[0] = -1;

    /// Rewrite the track ids so that they are consecutive.
    for (TG4TrajectoryContainer::iterator
             t = dest.begin();
         t != dest.end(); ++t) {
        t->TrackId = fTrackIdMap[t->TrackId];
        t->ParentId = fTrackIdMap[t->ParentId];
    }

}

void EDepSim::PersistencyManager::MarkTrajectories(const G4Event* event) {
    const G4TrajectoryContainer* trajectories = event->GetTrajectoryContainer();
    if (!trajectories) {
        EDepSimVerbose("No Trajectories ");
        return;
    }

    // Go through all of the trajectories and save:
    //
    //   ** Trajectories from primary particles if
    //       1) a daughter deposited energy in a sensitive detector
    //       2) or, SaveAllPrimaryTrajectories() is true
    //
    //   ** Trajectories created by a particle decay if
    //       1) a daughter deposited energy in a sensitve detector
    //       2) or, SaveAllPrimaryTrajectories() is true.
    //
    //   ** Charged particle trajectories that pass through a sensitive
    //         detector.
    //
    //   ** Gamma-rays and neutrons above a threshold which have daughters
    //         depositing energy in a sensitive detector.
    //
    for (TrajectoryVector::iterator t = trajectories->GetVector()->begin();
         t != trajectories->GetVector()->end();
         ++t) {
        EDepSim::Trajectory* ndTraj = dynamic_cast<EDepSim::Trajectory*>(*t);
        std::string particleName = ndTraj->GetParticleName();
        std::string processName = ndTraj->GetProcessName();
        double initialMomentum = ndTraj->GetInitialMomentum().mag();

        // Check if all primary particle trajectories should be saved.  The
        // primary particle should always be saved if it, or any of it's
        // children, deposited energy in a sensitive detector.  If the primary
        // didn't deposit energy in a sensitive detector, then it will only be
        // saved if SaveAllPrimaryTrajectories is true.
        if (ndTraj->GetParentID() == 0) {
            if (ndTraj->GetSDTotalEnergyDeposit()>1*eV
                || GetSaveAllPrimaryTrajectories()) {
                ndTraj->MarkTrajectory();
                continue;
            }
        }

        // Don't save the neutrinos
        if (particleName == "anti_nu_e") continue;
        if (particleName == "anti_nu_mu") continue;
        if (particleName == "anti_nu_tau") continue;
        if (particleName == "nu_e") continue;
        if (particleName == "nu_mu") continue;
        if (particleName == "nu_tau") continue;

        // Save any decay product if it caused any energy deposit.
        if (processName == "Decay") {
            if (ndTraj->GetSDTotalEnergyDeposit()>1*eV
                || GetSaveAllPrimaryTrajectories()) {
                ndTraj->MarkTrajectory(false);
                continue;
            }
        }

        // Save particles that produce charged track inside a sensitive
        // detector.  This doesn't automatically save, but the parents will be
        // automatically considered for saving by the next bit of code.
        if (ndTraj->GetSDLength() > GetLengthThreshold()) {
            ndTraj->MarkTrajectory(false);
            continue;
        }

        // For the next part, only consider particles where the children have
        // deposited energy in a sensitive detector.
        if (ndTraj->GetSDTotalEnergyDeposit()<1*eV) {
            continue;
        }

        // Save higher energy gamma rays that have descendents depositing
        // energy in a sensitive detector.  This only affects secondary
        // photons since primary photons are handled above.
        if (particleName == "gamma" && initialMomentum > GetGammaThreshold()) {
            ndTraj->MarkTrajectory(false);
            continue;
        }

        // Save higher energy neutrons that have descendents depositing energy
        // in a sensitive detector.  This only affects secondary neutrons
        // since primary neutrons are controlled above.
        if (particleName == "neutron"
            && initialMomentum > GetNeutronThreshold()) {
            ndTraj->MarkTrajectory(false);
            continue;
        }
    }

    // Go through all of the event hit collections and make sure that all
    // primary trajectories and trajectories contributing to a hit are saved.
    // These are mostly a sub-set of the trajectories marked in the previous
    // step, but there are a few corner cases where trajectories are not saved
    // because of theshold issues.
    G4HCofThisEvent* hitCollections = event->GetHCofThisEvent();
    if (!hitCollections) return;
    for (int i=0; i < hitCollections->GetNumberOfCollections(); ++i) {
        G4VHitsCollection* g4Hits = hitCollections->GetHC(i);
        if (g4Hits->GetSize()<1) continue;
        for (unsigned int h=0; h<g4Hits->GetSize(); ++h) {
            EDepSim::HitSegment* g4Hit
                = dynamic_cast<EDepSim::HitSegment*>(g4Hits->GetHit(h));
            if (!g4Hit) {
                EDepSimError("Not a hit segment");
                continue;
            }

            // Explicitly save the primary.  It will probably be marked again
            // with the contributors, but that's OK.  This catches some corner
            // cases where the primary isn't what you would expect.
            int primaryId = g4Hit->GetPrimaryTrajectoryId();
            EDepSim::Trajectory* ndTraj
                = dynamic_cast<EDepSim::Trajectory*>(
                    EDepSim::TrajectoryMap::Get(primaryId));
            if (ndTraj) {
                ndTraj->MarkTrajectory(false);
            }
            else {
                EDepSimError("Primary trajectory not found");
            }

            // Make sure that all the contributors associated with this hit
            // are saved.
            for (int j = 0; j < g4Hit->GetContributorCount(); ++j) {
                int contribId = g4Hit->GetContributor(j);
                EDepSim::Trajectory* contribTraj
                    = dynamic_cast<EDepSim::Trajectory*>(
                        EDepSim::TrajectoryMap::Get(contribId));
                if (contribTraj) {
                    contribTraj->MarkTrajectory(false);
                }
                else {
                    EDepSimError("Contributor trajectory not found");
                }
            }
        }
    }
}

void EDepSim::PersistencyManager::CopyTrajectoryPoints(TG4Trajectory& traj,
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
        EDepSim::TrajectoryPoint* edepPoint
            = dynamic_cast<EDepSim::TrajectoryPoint*>(g4Traj->GetPoint(*tp));
        TG4TrajectoryPoint point;
        point.Position.SetXYZT(edepPoint->GetPosition().x(),
                               edepPoint->GetPosition().y(),
                               edepPoint->GetPosition().z(),
                               edepPoint->GetTime());
        point.Momentum.SetXYZ(edepPoint->GetMomentum().x(),
                              edepPoint->GetMomentum().y(),
                              edepPoint->GetMomentum().z());
        point.Process = edepPoint->GetProcessType();
        point.Subprocess = edepPoint->GetProcessSubType();
        traj.Points.push_back(point);
    }
}

void
EDepSim::PersistencyManager::SummarizeSegmentDetectors(
    TG4HitSegmentDetectors& dest,
    const G4Event* event) {
    dest.clear();

    G4HCofThisEvent* HCofEvent = event->GetHCofThisEvent();
    if (!HCofEvent) return;
    G4SDManager *sdM = G4SDManager::GetSDMpointer();
    G4HCtable *hcT = sdM->GetHCtable();
    // Copy each of the hit categories into the output event.
    for (int i=0; i<hcT->entries(); ++i) {
        G4String SDname = hcT->GetSDname(i);
        G4String HCname = hcT->GetHCname(i);
        int HCId = sdM->GetCollectionID(SDname+"/"+HCname);
        G4VHitsCollection* g4Hits = HCofEvent->GetHC(HCId);
        if (g4Hits->GetSize()<1) continue;
        EDepSim::HitSegment* hitSeg
            = dynamic_cast<EDepSim::HitSegment*>(g4Hits->GetHit(0));
        if (!hitSeg) continue;
        SummarizeHitSegments(dest[SDname],g4Hits);
    }
}

void
EDepSim::PersistencyManager::SummarizeHitSegments(TG4HitSegmentContainer& dest,
                                             G4VHitsCollection* g4Hits) {
    dest.clear();

    EDepSim::HitSegment* g4Hit = dynamic_cast<EDepSim::HitSegment*>(g4Hits->GetHit(0));
    if (!g4Hit) return;

    for (std::size_t h=0; h<g4Hits->GetSize(); ++h) {
        g4Hit = dynamic_cast<EDepSim::HitSegment*>(g4Hits->GetHit(h));
        TG4HitSegment hit;
        hit.PrimaryId = fTrackIdMap[g4Hit->GetPrimaryTrajectoryId()];
        hit.EnergyDeposit = g4Hit->GetEnergyDeposit();
        hit.SecondaryDeposit = g4Hit->GetSecondaryDeposit();
        hit.TrackLength = g4Hit->GetTrackLength();
        CopyHitContributors(hit.Contrib,g4Hit->GetContributors());
        hit.Start.SetXYZT(g4Hit->GetStart().x(),
                          g4Hit->GetStart().y(),
                          g4Hit->GetStart().z(),
                          g4Hit->GetStart().t());
        hit.Stop.SetXYZT(g4Hit->GetStop().x(),
                          g4Hit->GetStop().y(),
                          g4Hit->GetStop().z(),
                          g4Hit->GetStop().t());
        dest.push_back(hit);
    }
}

void EDepSim::PersistencyManager::CopyHitContributors(std::vector<int>& dest,
                                                 const std::vector<int>& src) {

    dest.clear();

    for (std::vector<int>::const_iterator c = src.begin();
         c != src.end(); ++c) {
        // Check each contributor to make sure that it is a valid
        // trajectory.  If it isn't in the trajectory map, then set it
        // to a parent that is.
        EDepSim::Trajectory* ndTraj
            = dynamic_cast<EDepSim::Trajectory*>(
                EDepSim::TrajectoryMap::Get(*c));
        while (ndTraj && !ndTraj->SaveTrajectory()) {
            ndTraj = dynamic_cast<EDepSim::Trajectory*>(
                EDepSim::TrajectoryMap::Get(ndTraj->GetParentID()));
        }
        if (!ndTraj) {
            dest.push_back(-1);
            continue;
        }
        if (fTrackIdMap.find(ndTraj->GetTrackID()) != fTrackIdMap.end()) {
            dest.push_back(fTrackIdMap[ndTraj->GetTrackID()]);
        }
        else {
            EDepSimError("Contributor with unknown trajectory: "
                      << ndTraj->GetTrackID());
            dest.push_back(-2);
        }
    }

    // Remove the duplicate entries.
    std::sort(dest.begin(),dest.end());
    dest.erase(std::unique(dest.begin(),dest.end()),dest.end());
}

double EDepSim::PersistencyManager::FindTrajectoryAccuracy(
    G4VTrajectory* g4Traj, int point1, int point2) {
    if ((point2-point1) < 2) return 0;

    G4ThreeVector p1 = g4Traj->GetPoint(point1)->GetPosition();
    G4ThreeVector p2 = g4Traj->GetPoint(point2)->GetPosition();

    if ( (p2-p1).mag() < fTrajectoryPointAccuracy) return 0;

    G4ThreeVector dir = (p2-p1).unit();

    int step = (point2-point1)/10 + 1;

    double approach = 0.0;
    for (int p = point1+1; p<point2; p = p + step) {
        p2 = g4Traj->GetPoint(p)->GetPosition() - p1;
        approach = std::max((p2 - (dir*p2)*dir).mag(), approach);
    }

    return approach;
}

int EDepSim::PersistencyManager::SplitTrajectory(G4VTrajectory* g4Traj,
                                            int point1, int point2) {

    int point3 = 0.5*(point1 + point2);
    if (point3 <= point1) EDepSimThrow("Points too close to split");
    if (point2 <= point3) EDepSimThrow("Points too close to split");
    double bestAccuracy = FindTrajectoryAccuracy(g4Traj, point1, point3);

    bestAccuracy = std::max(FindTrajectoryAccuracy(g4Traj, point3, point2),
                            bestAccuracy);
    for (int p = point1+1; p<point2-1; ++p) {
        double a1 = FindTrajectoryAccuracy(g4Traj,point1, p);
        double a2 = FindTrajectoryAccuracy(g4Traj,p, point2);
        double accuracy = std::max(a1,a2);
        if (accuracy < bestAccuracy) {
            point3 = p;
            bestAccuracy  = accuracy;
        }
    }

    return point3;
}

void
EDepSim::PersistencyManager::SelectTrajectoryPoints(std::vector<int>& selected,
                                                    G4VTrajectory* g4Traj) {

    selected.clear();
    if (g4Traj->GetPointEntries() < 1) {
        EDepSimError("Trajectory with no points"
                     << " " << g4Traj->GetTrackID()
                     << " " << g4Traj->GetParentID()
                     << " " << g4Traj->GetParticleName());
        return;
    }

    ////////////////////////////////////
    // Save the first point of the trajectory.
    ////////////////////////////////////
    selected.push_back(0);

    /////////////////////////////////////
    // Save the last point of the trajectory.
    /////////////////////////////////////
    int lastIndex = g4Traj->GetPointEntries()-1;
    if (lastIndex < 1) {
        EDepSimError("Trajectory with one point"
                     << " " << g4Traj->GetTrackID()
                     << " " << g4Traj->GetParentID()
                     << " " << g4Traj->GetParticleName());
        return;
    }
    selected.push_back(lastIndex);

    //////////////////////////////////////////////
    // Short out trajectories that don't create any energy deposit in a
    // sensitive detector.  These are trajectories that disappear from the
    // detector, so we don't need to record the extra trajectory points.  The
    // starting and stopping point of the particle are recorded in the
    // trajectory.
    //////////////////////////////////////////////
    EDepSim::Trajectory* ndTraj = dynamic_cast<EDepSim::Trajectory*>(g4Traj);
    if (ndTraj->GetSDTotalEnergyDeposit() < 1*eV) return;

    // Find the trajectory points where particles are entering and leaving the
    // detectors.
    EDepSim::TrajectoryPoint* edepPoint
        = dynamic_cast<EDepSim::TrajectoryPoint*>(g4Traj->GetPoint(0));
    G4String prevVolumeName = edepPoint->GetPhysVolName();
    for (int tp = 1; tp < lastIndex; ++tp) {
        edepPoint
            = dynamic_cast<EDepSim::TrajectoryPoint*>(g4Traj->GetPoint(tp));
        G4String volumeName = edepPoint->GetPhysVolName();
        // Save the point on a boundary crossing for volumes where we are
        // saving the entry and exit points.
        if (SaveTrajectoryBoundary(g4Traj,edepPoint->GetStepStatus(),
                                   volumeName,prevVolumeName)) {
            selected.push_back(tp);
        }
        prevVolumeName = volumeName;
    }

    // Save trajectory points where there is a "big" interaction.
    for (int tp = 1; tp < lastIndex; ++tp) {
        edepPoint
            = dynamic_cast<EDepSim::TrajectoryPoint*>(g4Traj->GetPoint(tp));
        // Just navigation....
        if (edepPoint->GetProcessType() == fTransportation) continue;
        // Not much energy deposit...
        if (edepPoint->GetProcessDeposit() < GetTrajectoryPointDeposit())
            continue;
        // Don't save optical photons...
        if (edepPoint->GetProcessType() == fOptical) continue;
        // Not a physics step...
        if (edepPoint->GetProcessType() == fGeneral) continue;
        if (edepPoint->GetProcessType() == fUserDefined) continue;
        // Don't save continuous ionization steps.
        if (edepPoint->GetProcessType() == fElectromagnetic
            && edepPoint->GetProcessSubType() == fIonisation) continue;
        // Don't save multiple scattering.
        if (edepPoint->GetProcessType() == fElectromagnetic
            && edepPoint->GetProcessSubType() == fMultipleScattering) continue;
        selected.push_back(tp);
    }

    // Make sure there aren't any duplicates in the selected trajectory points.
    std::sort(selected.begin(), selected.end());
    selected.erase(std::unique(selected.begin(), selected.end()),
                   selected.end());

    if (ndTraj->GetSDEnergyDeposit() < 1*eV) return;

    double desiredAccuracy = GetTrajectoryPointAccuracy();
    // Make sure that the trajectory accuracy stays in tolerance.
    for (int throttle = 0; throttle < 1000; ++throttle) {
        bool addPoint = false;
        for (std::vector<int>::iterator p1 = selected.begin();
             p1 != selected.end();
             ++p1) {
            std::vector<int>::iterator p2 = p1+1;
            if (p2==selected.end()) break;
            double trajectoryAccuracy = FindTrajectoryAccuracy(g4Traj,*p1,*p2);
            if (trajectoryAccuracy <= desiredAccuracy) continue;
            int split = SplitTrajectory(g4Traj,*p1,*p2);
            if (split < 0) continue;
            selected.push_back(split);
            addPoint = true;
            break;
        }
        std::sort(selected.begin(), selected.end());
        selected.erase(std::unique(selected.begin(), selected.end()),
                       selected.end());
        if (!addPoint) break;
    }
}
