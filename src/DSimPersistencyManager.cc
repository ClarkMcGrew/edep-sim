//
// Implementation for concrete G4PersistencyManager.
//

#include "DSimPersistencyManager.hh"
#include "DSimPersistencyMessenger.hh"
#include "DSimTrajectory.hh"
#include "DSimTrajectoryPoint.hh"
#include "DSimTrajectoryMap.hh"
#include "DSimHitSegment.hh"
#include "DSimException.hh"

#include <G4ios.hh>
#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4StepStatus.hh>

#include <TPRegexp.h>

// Creator for the persistency manager.  This is NOT a public routine
// since it is only used by this class to create a singleton object.
DSimPersistencyManager::DSimPersistencyManager() 
    : G4VPersistencyManager(), fFilename("/dev/null"),
      fLengthThreshold(10*mm),
      fGammaThreshold(5*MeV), fNeutronThreshold(50*MeV),
      fTrajectoryPointAccuracy(1.*mm), fSaveAllPrimaryTrajectories(true) {
    fPersistencyMessenger = new DSimPersistencyMessenger(this);
}

// When should this be called???  Probably the user should never call
// this directly and it will be called automatically in the exit phase.
DSimPersistencyManager::~DSimPersistencyManager() {
    ClearTrajectoryBoundaries();
    delete fPersistencyMessenger;
}

G4bool DSimPersistencyManager::Store(const G4Event* anEvent) {
    DSimSevere(" -- Event store called without a save method " 
                << GetFilename());
    return false;
}

G4bool DSimPersistencyManager::Store(const G4Run* aRun) {
    DSimSevere(" -- Run store called without a save method " 
                << GetFilename());
    return false;
}

G4bool DSimPersistencyManager::Store(const G4VPhysicalVolume* aWorld) {
    DSimSevere(" -- Geometry store called without a save method " 
                << GetFilename());
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

        // Save particles that produce a signal in a sensitive detector.  This
        // doesn't automatically save, but the parents will be automatically
        // considered for saving by the next bit of code.
        if (ndTraj->GetSDLength() > GetLengthThreshold()) {
            ndTraj->MarkTrajectory(false);
            continue;
        }

        // For the next part, only consider particles where the children have
        // deposited energy in a sensitive detector.
        if (ndTraj->GetSDTotalEnergyDeposit()<1*eV) continue;

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
                DSimSevere("Not a hit segment");
                continue;
            }

            // Make sure that the primary trajectory associated with this hit
            // is saved.  The primary trajectories are defined by
            // DSimTrajectoryMap::FindPrimaryId().
            int primaryId = g4Hit->GetPrimaryTrajectoryId();
            DSimTrajectory* ndTraj 
                = dynamic_cast<DSimTrajectory*>(
                    DSimTrajectoryMap::Get(primaryId));
            if (ndTraj) ndTraj->MarkTrajectory(false);
            else DSimSevere("Primary trajectory not found");

        }
    }
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

int DSimPersistencyManager::SplitTrajectory(
    G4VTrajectory* g4Traj, int point1, int point2) {

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

void DSimPersistencyManager::SelectTrajectoryPoints(
    std::vector<int>& selected, G4VTrajectory* g4Traj) {

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
        // Not much energy deposit...
        if (dsimPoint->GetProcessDeposit() < 1.0*MeV) continue;
        // Just navigation....
        if (dsimPoint->GetProcessType() == fTransportation) continue;
        // Don't save optical photons...
        if (dsimPoint->GetProcessType() == fOptical) continue;
        // Usually a step limit...
        if (dsimPoint->GetProcessType() == fGeneral) continue;
        if (dsimPoint->GetProcessType() == fUserDefined) continue;
        // Don't save continuous ionization steps.
        if (dsimPoint->GetProcessName().find("Ioni")!=G4String::npos) continue;
        // Don't save multiple scattering.
        if (dsimPoint->GetProcessName().find("msc")!=G4String::npos) continue;
        selected.push_back(tp);
    }

    // Make sure there aren't any accidental duplicates in selected.
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
