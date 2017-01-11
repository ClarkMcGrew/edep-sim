//
// Implementation for concrete G4PersistencyManager.
//

#include <TPRegexp.h>

#include "DSimPersistencyManager.hh"
#include "DSimPersistencyMessenger.hh"
#include "DSimVertexInfo.hh"
#include "DSimTrajectory.hh"
#include "DSimTrajectoryPoint.hh"
#include "DSimTrajectoryMap.hh"
#include "DSimHitSegment.hh"
#include "DSimException.hh"
#include "DSimUserRunAction.hh"

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
DSimPersistencyManager::DSimPersistencyManager() 
    : G4VPersistencyManager(), fFilename("/dev/null"),
      fLengthThreshold(10*mm),
      fGammaThreshold(5*MeV), fNeutronThreshold(50*MeV),
      fTrajectoryPointAccuracy(1.*mm), fSaveAllPrimaryTrajectories(true) {
    fPersistencyMessenger = new DSimPersistencyMessenger(this);
}

// This is called by the G4RunManager during AnalyzeEvent.
DSimPersistencyManager::~DSimPersistencyManager() {
    ClearTrajectoryBoundaries();
    delete fPersistencyMessenger;
}

G4bool DSimPersistencyManager::Open(G4String filename) {
    DSimSevere(" -- Open is not implimented for " << filename);
    SetFilename(filename);
    return false;
}

/// Make sure the output file is closed.
G4bool DSimPersistencyManager::Close(void) {
    DSimSevere(" -- Close is not implimented.");
    return false;
}

// This is called by the G4RunManager during AnalyzeEvent.
G4bool DSimPersistencyManager::Store(const G4Event* anEvent) {
    UpdateSummaries(anEvent);
    return false;
}

// This is called by the G4RunManager during AnalyzeEvent.
G4bool DSimPersistencyManager::Store(const G4Run* aRun) {
    if (!aRun) return false;
    DSimSevere(" -- Run store called without a save method for " 
               << "GEANT4 run " << aRun->GetRunID());
    return false;
}

// This is called by the G4RunManager during AnalyzeEvent.
G4bool DSimPersistencyManager::Store(const G4VPhysicalVolume* aWorld) {
    if (!aWorld) return false;
    DSimSevere(" -- Geometry store called without a save method for "
               << aWorld->GetName());
    return false;
}

void DSimPersistencyManager::AddTrajectoryBoundary(const G4String& b) {
    TPRegexp* bound = new TPRegexp(b.c_str());
    fTrajectoryBoundaries.push_back(bound);
}

void DSimPersistencyManager::ClearTrajectoryBoundaries() {
    for (std::vector<TPRegexp*>::iterator r = fTrajectoryBoundaries.begin();
         r != fTrajectoryBoundaries.end();
         ++r) {
        delete (*r);
    }
    fTrajectoryBoundaries.clear();
}

bool DSimPersistencyManager::SaveTrajectoryBoundary(G4VTrajectory* g4Traj,
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
            DSimNamedDebug("boundary","Entering " << current);
            return true;
        }
        // Check if a watched volume is being exited.
        if ((*r)->Match(current)<1 && (*r)->Match(previous)>0) {
            DSimNamedDebug("boundary","Exiting " << current);
            return true;
        }
    }
    return false;
}

void DSimPersistencyManager::UpdateSummaries(const G4Event* event) {
    
    const G4Run* runInfo = G4RunManager::GetRunManager()->GetCurrentRun();

    fEventSummary.RunId = runInfo->GetRunID();
    fEventSummary.EventId = event->GetEventID();

    // Summarize the trajectories first so that fTrackIdMap is filled.
    MarkTrajectories(event);

    SummarizePrimaries(fEventSummary.Primaries,event->GetPrimaryVertex());

    SummarizeTrajectories(fEventSummary.Trajectories,event);

    SummarizeSegmentDetectors(fEventSummary.SegmentDetectors, event);
}

void DSimPersistencyManager::SummarizePrimaries(
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
        
#define SUMMARIZE_PARTICLES
#ifdef SUMMARIZE_PARTICLES
        for (int i=0; i< src->GetNumberOfParticle(); ++i) {
            TG4PrimaryParticle prim;
            G4PrimaryParticle *g4Prim = src->GetPrimary(i);
            prim.Name = g4Prim->GetG4code()->GetParticleName();
            prim.PDGCode = g4Prim->GetPDGcode();
            prim.TrackId = g4Prim->GetTrackID() - 1;
            prim.Momentum.SetX(g4Prim->GetPx());
            prim.Momentum.SetY(g4Prim->GetPy());
            prim.Momentum.SetZ(g4Prim->GetPz());
            double E = pow(prim.Momentum.P(),2) 
                + pow(g4Prim->GetG4code()->GetPDGMass(),2);
            if (E>0) E = std::sqrt(E);
            else E = 0;
            prim.Momentum.SetE(E);
            vtx.Particles.push_back(prim);
        }
#endif
        
        DSimVertexInfo* srcInfo 
            = dynamic_cast<DSimVertexInfo*>(src->GetUserInformation());
        if (srcInfo) {
            vtx.GeneratorName = srcInfo->GetName();
            vtx.Reaction = srcInfo->GetReaction();
            vtx.Filename = srcInfo->GetFilename();
            vtx.InteractionNumber = srcInfo->GetInteractionNumber();
            vtx.CrossSection = srcInfo->GetCrossSection();
            vtx.DiffCrossSection = srcInfo->GetDiffCrossSection();
            vtx.Weight = srcInfo->GetWeight();
            vtx.Probability = srcInfo->GetProbability();
#ifdef SUMMARY_INFO_VERTEX
            if (srcInfo->GetInformationalVertex()) {
                SummarizePrimaries(vtx.Informational,
                                   srcInfo->GetInformationalVertex());
            }
#endif
        }

        dest.push_back(vtx);
        src = src->GetNext();
    }
}

void DSimPersistencyManager::SummarizeTrajectories(
    TG4TrajectoryContainer& dest,
    const G4Event* event) {
    dest.clear();
    MarkTrajectories(event);
    
    const G4TrajectoryContainer* trajectories = event->GetTrajectoryContainer();

    // Build a map of the original G4 TrackID to the new relocated TrackId
    // (not capitalization).  This also uses the fact that maps are sorted as
    // a hack to prevent writing a predicate to sort TG4Trajectories (because
    // I'm lazy).
    fTrackIdMap.clear();

    int index = 0;
    TG4TrajectoryContainer tempContainer;
    for (TrajectoryVector::iterator t = trajectories->GetVector()->begin();
         t != trajectories->GetVector()->end();
         ++t) {
        DSimTrajectory* ndTraj = dynamic_cast<DSimTrajectory*>(*t);

        // Check if the trajectory should be saved.
        if (!ndTraj->SaveTrajectory()) continue;

        // Set the particle type information.
        G4ParticleDefinition* part 
            = G4ParticleTable::GetParticleTable()->FindParticle(
                ndTraj->GetParticleName());
        if (!part) {
            DSimError(std::string("DSimRootPersistencyManager::")
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
            DSimTrajectory* pTraj
                = dynamic_cast<DSimTrajectory*>(
                    DSimTrajectoryMap::Get(traj.ParentId));
            if (!pTraj) {
                DSimError("Trajectory " << traj.ParentId << " does not exist");
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

    // TrackID zero maps to the non-existent -1.
    fTrackIdMap[0] = -1;
    
    /// Rewrite the track ids so that they are consecutive.
    for (TG4TrajectoryContainer::iterator
             t = dest.begin();
         t != dest.end(); ++t) {
        t->TrackId = fTrackIdMap[t->TrackId];
        t->ParentId = fTrackIdMap[t->ParentId];
    }
  
}    

void DSimPersistencyManager::MarkTrajectories(const G4Event* event) {
    const G4TrajectoryContainer* trajectories = event->GetTrajectoryContainer();
    if (!trajectories) {
        DSimVerbose("No Trajectories ");
        return;
    }

    // Go through all of the trajectories and save:
    //
    //   ** Trajectories from primary particles if
    //       1) a daughter depeosited energy in a sensitive detector
    //       2) or, SaveAllPrimaryTrajectories() is true
    //
    //   ** Trajectories created by a particle decay.
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
        DSimTrajectory* ndTraj = dynamic_cast<DSimTrajectory*>(*t);
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

        // Save any decay product
        if (processName == "Decay") {
            ndTraj->MarkTrajectory(false);
            continue;
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
        // energy in a sensitive detector.
        if (particleName == "gamma" && initialMomentum > GetGammaThreshold()) {
            ndTraj->MarkTrajectory(false);
            continue;
        }

        // Save higher energy neutrons that have descendents depositing energy
        // in a sensitive detector.
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
            DSimHitSegment* g4Hit
                = dynamic_cast<DSimHitSegment*>(g4Hits->GetHit(h));
            if (!g4Hit) {
                DSimError("Not a hit segment");
                continue;
            }

            // Make sure that the primary trajectory associated with this hit
            // is saved.  The primary trajectories are defined by
            // DSimTrajectoryMap::FindPrimaryId().
            int primaryId = g4Hit->GetPrimaryTrajectoryId();
            DSimTrajectory* ndTraj 
                = dynamic_cast<DSimTrajectory*>(
                    DSimTrajectoryMap::Get(primaryId));
            if (ndTraj) {
                ndTraj->MarkTrajectory(false);
            }
            else {
                DSimError("Primary trajectory not found");
            }
        }
    }
}

void DSimPersistencyManager::CopyTrajectoryPoints(TG4Trajectory& traj, 
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
        TG4TrajectoryPoint point;
        point.Position.SetXYZT(dsimPoint->GetPosition().x(),
                               dsimPoint->GetPosition().y(),
                               dsimPoint->GetPosition().z(),
                               dsimPoint->GetTime());
        point.Momentum.SetXYZ(dsimPoint->GetMomentum().x(),
                              dsimPoint->GetMomentum().y(),
                              dsimPoint->GetMomentum().z());
        point.Process = dsimPoint->GetProcessType();
        point.Subprocess = dsimPoint->GetProcessSubType();
        traj.Points.push_back(point);
    }
}

void
DSimPersistencyManager::SummarizeSegmentDetectors(
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
        DSimHitSegment* hitSeg
            = dynamic_cast<DSimHitSegment*>(g4Hits->GetHit(0));
        if (!hitSeg) continue;
        SummarizeHitSegments(dest[SDname],g4Hits);
    }
}

void
DSimPersistencyManager::SummarizeHitSegments(TG4HitSegmentContainer& dest,
                                             G4VHitsCollection* g4Hits) {
    dest.clear();

    DSimHitSegment* g4Hit = dynamic_cast<DSimHitSegment*>(g4Hits->GetHit(0));
    if (!g4Hit) return;

    for (std::size_t h=0; h<g4Hits->GetSize(); ++h) {
        g4Hit = dynamic_cast<DSimHitSegment*>(g4Hits->GetHit(h));
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

void DSimPersistencyManager::CopyHitContributors(std::vector<int>& dest,
                                                 const std::vector<int>& src) {
    
    dest.clear();
    
    for (std::vector<int>::const_iterator c = src.begin();
         c != src.end(); ++c) {
        // Check each contributor to make sure that it is a valid
        // trajectory.  If it isn't in the trajectory map, then set it
        // to a parent that is.
        DSimTrajectory* ndTraj
            = dynamic_cast<DSimTrajectory*>(DSimTrajectoryMap::Get(*c));
        while (ndTraj && !ndTraj->SaveTrajectory()) {
            ndTraj = dynamic_cast<DSimTrajectory*>(
                DSimTrajectoryMap::Get(ndTraj->GetParentID()));
        }
        if (!ndTraj) {
            dest.push_back(-1);
            continue;
        }
        if (fTrackIdMap.find(ndTraj->GetTrackID()) != fTrackIdMap.end()) {
            dest.push_back(fTrackIdMap[ndTraj->GetTrackID()]);
        }
        else {
            DSimError("Contributor with unknown trajectory: "
                      << ndTraj->GetTrackID());
            dest.push_back(-2);
        }
    }

    // Remove the duplicate entries.
    std::sort(dest.begin(),dest.end());
    dest.erase(std::unique(dest.begin(),dest.end()),dest.end());
}

double DSimPersistencyManager::FindTrajectoryAccuracy(
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

int DSimPersistencyManager::SplitTrajectory(G4VTrajectory* g4Traj,
                                            int point1, int point2) {

    int point3 = 0.5*(point1 + point2);
    if (point3 <= point1) DSimThrow("Points too close to split");
    if (point2 <= point3) DSimThrow("Points too close to split");
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
DSimPersistencyManager::SelectTrajectoryPoints(std::vector<int>& selected,
                                               G4VTrajectory* g4Traj) {

    selected.clear();
    if (g4Traj->GetPointEntries() < 1) return;

    ////////////////////////////////////
    // Save the first point of the trajectory.
    ////////////////////////////////////
    selected.push_back(0);

    /////////////////////////////////////
    // Save the last point of the trajectory.
    /////////////////////////////////////
    int lastIndex = g4Traj->GetPointEntries()-1;
    if (lastIndex < 1) return;
    selected.push_back(lastIndex);

    //////////////////////////////////////////////
    // Short out trajectories that don't create any energy deposit in a
    // sensitive detector.  These are trajectories that disappear from the
    // detector, so we don't need to record the extra trajectory points.  The
    // starting and stopping point of the particle are recorded in the
    // trajectory.
    //////////////////////////////////////////////
    DSimTrajectory* ndTraj = dynamic_cast<DSimTrajectory*>(g4Traj);    
    if (ndTraj->GetSDTotalEnergyDeposit() < 1*eV) return;

    // Find the trajectory points where particles are entering and leaving the
    // detectors.
    DSimTrajectoryPoint* dsimPoint 
        = dynamic_cast<DSimTrajectoryPoint*>(g4Traj->GetPoint(0));
    G4String prevVolumeName = dsimPoint->GetPhysVolName();
    for (int tp = 1; tp < lastIndex; ++tp) {
        dsimPoint = dynamic_cast<DSimTrajectoryPoint*>(g4Traj->GetPoint(tp));
        G4String volumeName = dsimPoint->GetPhysVolName();
        // Save the point on a boundary crossing for volumes where we are
        // saving the entry and exit points.
        if (SaveTrajectoryBoundary(g4Traj,dsimPoint->GetStepStatus(),
                                   volumeName,prevVolumeName)) {
            selected.push_back(tp);
        }
        prevVolumeName = volumeName;
    }

    // Save trajectory points where there is a "big" interaction.
    for (int tp = 1; tp < lastIndex; ++tp) {
        dsimPoint = dynamic_cast<DSimTrajectoryPoint*>(g4Traj->GetPoint(tp));
        // Just navigation....
        if (dsimPoint->GetProcessType() == fTransportation) continue;
        // Not much energy deposit...
        if (dsimPoint->GetProcessDeposit() < 0.5*MeV) continue;
        // Don't save optical photons...
        if (dsimPoint->GetProcessType() == fOptical) continue;
        // Not a physics step...
        if (dsimPoint->GetProcessType() == fGeneral) continue;
        if (dsimPoint->GetProcessType() == fUserDefined) continue;
        // Don't save continuous ionization steps.
        if (dsimPoint->GetProcessType() == fElectromagnetic
            && dsimPoint->GetProcessSubType() == fIonisation) continue;
        // Don't save multiple scattering.
        if (dsimPoint->GetProcessType() == fElectromagnetic
            && dsimPoint->GetProcessSubType() == fMultipleScattering) continue;
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
