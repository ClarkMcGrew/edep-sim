////////////////////////////////////////////////////////////
// $Id: EDepSim::UserTrackingAction.cc,v 1.6 2007/01/01 05:32:49 mcgrew Exp $
//
#include "EDepSimUserTrackingAction.hh"
#include "EDepSimTrajectory.hh"
#include "EDepSimTrajectoryMap.hh"
#include "EDepSimLog.hh"
#include "EDepSimBacktrace.hh"

#include <globals.hh>
#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4VSensitiveDetector.hh>
#include <G4SystemOfUnits.hh>

EDepSim::UserTrackingAction::UserTrackingAction()
    : fOpticalPhotonOpBoundaryProcessCached(false),
      fOpticalPhotonOpBoundaryProcess(nullptr),
      fSavePhotonTrajectories(false) {}

EDepSim::UserTrackingAction::~UserTrackingAction() {}

void
EDepSim::UserTrackingAction::PreUserTrackingAction(const G4Track* theTrack) {
    int trackId = theTrack->GetTrackID();
    EDepSimTrace("Pre-tracking action for " << trackId);

    // Run the external actions first.  These must not change the state of G4,
    // or EDepSim.
    for (G4UserTrackingAction *action : fExternalActions) {
        action->PreUserTrackingAction(theTrack);
    }

    if (theTrack->GetParticleDefinition()->GetPDGEncoding() == -22
        and not fSavePhotonTrajectories) {
        // Don't save optical photon trajectories, for they are many, and not
        // particularly useful.
        fpTrackingManager->SetStoreTrajectory(false);
        return;
    }

    // Make a new trajectory and give it to the tracking manager.  This might
    // be a continuation of an existing track, but that is OK since the
    // trajectories will be merged later on.
    G4VTrajectory* traj = new EDepSim::Trajectory(theTrack);
    fpTrackingManager->SetTrajectory(traj);
    fpTrackingManager->SetStoreTrajectory(true);

    // Check if the trajectory is in the map already. If it is in the map,
    // then the track has already been started and this is a continuation of
    // an existing track.
    G4VTrajectory* oldTraj = EDepSim::TrajectoryMap::Get(trackId);

    if (oldTraj == nullptr) {
        // There isn't an old trajectory, so add the new one to the map.
        EDepSim::TrajectoryMap::Add(traj);
    }
    else {
        // Sanity check.  The old trajectory must be an EDepSim::Trajectory
        EDepSim::Trajectory *update
            = dynamic_cast<EDepSim::Trajectory*>(oldTraj);
        if (update == nullptr) {
            EDepSimError("Not an EDepSim Trajectory " << std::endl
                         << EDepSim::Backtrace);
            throw;
        }
    }
}

void
EDepSim::UserTrackingAction::PostUserTrackingAction(const G4Track* theTrack) {
    EDepSimTrace("Post-tracking action for " << theTrack->GetTrackID());

    // Run the external actions first.  These must not change the state of G4
    // or EDepSim.
    for (G4UserTrackingAction *action : fExternalActions) {
        action->PostUserTrackingAction(theTrack);
    }

    const G4Step* theStep = theTrack->GetStep();
    if (theStep == nullptr) {
        EDepSimDebug("Missing step information");
        return;
    }

    const G4StepPoint* thePreStep = theStep->GetPreStepPoint();
    const G4StepPoint* thePostStep = theStep->GetPostStepPoint();
    if (thePreStep == nullptr or thePostStep == nullptr) {
        EDepSimDebug("Missing step point information");
        return;
    }

    const G4VPhysicalVolume* thePrePV = thePreStep->GetPhysicalVolume();
    const G4VPhysicalVolume* thePostPV = thePostStep->GetPhysicalVolume();

    G4String theCurrentVolumeName{"MissingVolume"};
    if (thePrePV != nullptr) theCurrentVolumeName = thePrePV->GetName();

    G4String theNextVolumeName{"MissingVolume"};
    if (thePostPV != nullptr) theNextVolumeName = thePostPV->GetName();

    const G4VProcess* theProcess = thePostStep->GetProcessDefinedStep();
    if (theProcess == nullptr) {
        EDepSimDebug("No process information");
        return;
    }

    G4String theProcessName = theProcess->GetProcessName()
        + "/" + theProcess->GetProcessTypeName(theProcess->GetProcessType());

    EDepSimTrace("Process " << theProcessName
                 << " deposit " << theStep->GetTotalEnergyDeposit()
                 << " status " << thePostStep->GetStepStatus());
    EDepSimTrace("    From " << theCurrentVolumeName);
    EDepSimTrace("    To   " << theNextVolumeName);

    const G4ParticleDefinition* theParticle = theTrack->GetParticleDefinition();

    // If the particle is not an optical photon, everything is done. (-22 is
    // the G4 convention for optical photons)
    if (theParticle->GetPDGEncoding() != -22) return;

    // Stop if the photon is out of this world.
    if (thePostPV == nullptr) return;

    // If not at a geometric boundary, everything is done.
    if (thePostStep->GetStepStatus() != G4StepStatus::fGeomBoundary) return;
    EDepSimTrace("Optical photon at boundary");

    if (not fOpticalPhotonOpBoundaryProcessCached) {
        EDepSimTrace("Cache boundary process");
        fOpticalPhotonOpBoundaryProcessCached = true;
        fOpticalPhotonOpBoundaryProcess
            = dynamic_cast<G4OpBoundaryProcess*>(
                theParticle->GetProcessManager()->GetProcess("OpBoundary"));
    }

    if (fOpticalPhotonOpBoundaryProcess == nullptr) {
        EDepSimTrace("No boundary process");
        return;
    }

    EDepSimTrace("Found boundary process -- status "
                 << fOpticalPhotonOpBoundaryProcess->GetStatus());

    // If the photon is not detected, everything is done
    if (fOpticalPhotonOpBoundaryProcess->GetStatus()
        != G4OpBoundaryProcessStatus::Detection) return;

    EDepSimTrace("Photon was detected");

#ifdef EXPLICITLY_REGISTER_HIT_WITH_SENSITIVE_DETECTOR
    // Register the hit based on it's final position (and any other
    // information).  This in case the SD cannot be invoked as normal for the
    // logical surface (i.e. G4OpticalParameters::SetInvokeBoundarySD is
    // false).
    G4ThreeVector hitPos = thePostStep->GetPosition();
    G4double hitTime = thePostStep->GetGlobalTime();
    G4ThreeVector hitPolar = thePostStep->GetPolarization();
    G4double hitEnergy = thePostStep->GetTotalEnergy();

    EDepSimDebug("Photon Hit"
                 << " " << hitTime/ns << " ns"
                 << " [" << hitPos.x()/cm << " cm"
                 << "," << hitPos.y()/cm << " cm"
                 << "," << hitPos.z()/cm << " cm]"
                 << " " << hitEnergy);
#endif

}
