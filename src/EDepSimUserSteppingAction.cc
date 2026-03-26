#include "EDepSimUserSteppingAction.hh"
#include "EDepSimLog.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4StepPoint.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4VProcess.hh>

EDepSim::SteppingAction::SteppingAction()
    : fStenchAndRot(0), fSteps(0), fThrottle(1000), fGovernor(0) {}

void EDepSim::SteppingAction::UserSteppingAction(const G4Step* theStep) {

    // Run the external actions first.  These must not change the state of G4,
    // or EDepSim.
    for (G4UserSteppingAction *action : fExternalActions) {
        action->UserSteppingAction(theStep);
    }

    G4Track* theTrack = theStep->GetTrack();

    const G4StepPoint* thePreStep = theStep->GetPreStepPoint();
    const G4StepPoint* thePostStep = theStep->GetPostStepPoint();

    const G4VPhysicalVolume* thePrePV = thePreStep->GetPhysicalVolume();
    const G4VPhysicalVolume* thePostPV = thePostStep->GetPhysicalVolume();

    G4String theParticleName = theTrack->GetDefinition()->GetParticleName();

    G4String theCurrentVolumeName{"MissingVolume"};
    if (thePrePV != nullptr) theCurrentVolumeName = thePrePV->GetName();

    G4String theNextVolumeName{"MissingVolume"};
    if (thePostPV != nullptr) theNextVolumeName = thePostPV->GetName();

    const G4VProcess* theProcess = thePostStep->GetProcessDefinedStep();

    G4String theProcessName{"UnknownProcess"};
    if (theProcess != nullptr) {
        theProcessName = theProcess->GetProcessName()
            + "/"
            + theProcess->GetProcessTypeName(theProcess->GetProcessType());
    }

    EDepSimTrace("Stepping " << theTrack->GetTrackID()
                 << " (step " << theTrack->GetCurrentStepNumber() << ")"
                 << " " << theProcessName
                 << " deposit " << theStep->GetTotalEnergyDeposit());
    EDepSimTrace("    From " << theCurrentVolumeName);
    EDepSimTrace("    To   " << theNextVolumeName);

    if (thePostPV == nullptr) {
        // Out of this world!
        theTrack->SetTrackStatus(fStopAndKill);
        EDepSimSevere("Stop and kill track that leaves world volume");
        fStenchAndRot = 0;
        fSteps = 0;
        fThrottle = 1000;
        return;
    }

    if (fSteps%fThrottle == 0 && fSteps>0) {
        EDepSimWarn("EDepSimUserSteppingAction:: Excessive Steps "
                  << " " << fSteps
                  << " for " << theParticleName
                  << " Length: " << theTrack->GetTrackLength()/m << " m"
                  << " Time: " << theTrack->GetGlobalTime()/ns << " ns"
                  << " Energy: " << theTrack->GetTotalEnergy()/MeV << " MeV"
                  << " Volume: " << theCurrentVolumeName);
        fThrottle *= 2;
        fGovernor = 5;
    }
    if (fGovernor>0) {
        // Print a few steps to help the user see what is happening.
        --fGovernor;
        EDepSimDebug("    " << theCurrentVolumeName
                   << ": " << theParticleName
                   << " -- Step: " << theStep->GetStepLength()/mm << " mm"
                   << " Energy Loss: "<< theStep->GetTotalEnergyDeposit()/MeV
                   << " MeV");
    }

    if (theStep->GetTrack()->GetTrackLength() > 1000*m) {
        theTrack->SetTrackStatus(fStopAndKill);
        EDepSimSevere("Stop and kill a very long track for "
                    << theParticleName << " w/ "
                    << G4BestUnit(theTrack->GetKineticEnergy(),"Energy")
                    << " (" << G4BestUnit(theTrack->GetTrackLength(),
                                          "Length")
                    << ")");
        fStenchAndRot = 0;
        fSteps = 0;
        fThrottle = 1000;
        return;
    }

    ++fSteps;
    if (fSteps>50000) {
        if (theTrack->GetKineticEnergy() > 1*MeV) {
            theTrack->SetTrackStatus(fStopAndKill);
            EDepSimSevere("Stop and kill track w/ too many steps for "
                        << theParticleName << " in "
                        << theCurrentVolumeName << " w/ "
                        << G4BestUnit(theTrack->GetKineticEnergy(),"Energy"));
        }
        else {
            theTrack->SetTrackStatus(fStopButAlive);
            EDepSimSevere("Stop w/ too many steps for "
                        << theParticleName << " in "
                        << theCurrentVolumeName << " w/ "
                        << G4BestUnit(theTrack->GetKineticEnergy(),"Energy"));
        }
        fStenchAndRot = 0;
        fSteps = 0;
        fThrottle = 1000;
        return;
    }

    if (theTrack->GetTrackStatus()==fStopAndKill) {
        // The track is stopped so this is the last step.
        fStenchAndRot = 0;
        fSteps = 0;
        fThrottle = 1000;
        return;
    }

    // A sequence of very short steps are bad, so keep track of the number of
    // short steps in a row.
    if (theStep->GetStepLength()>1*nanometer) {
        fStenchAndRot = 0;
        return;
    }

    // Count the number of bad steps in a row we've had, and stop the particle
    // if it looks like it's stuck.
    ++fStenchAndRot;
    if (fStenchAndRot<100) {
        return;
    }

    if (theTrack->GetKineticEnergy() > 1*MeV) {
        theTrack->SetTrackStatus(fStopAndKill);
        EDepSimSevere("Stop and kill stuck "
                    << theParticleName << " in "
                    << theCurrentVolumeName << " w/ "
                    << G4BestUnit(theTrack->GetKineticEnergy(),"Energy")
                    << " MeV");
        fStenchAndRot = 0;
        fSteps = 0;
        fThrottle = 1000;
        return;
    }

    theTrack->SetTrackStatus(fStopButAlive);
    EDepSimSevere("Stop stuck track "
                  << theParticleName << " in "
                  << theCurrentVolumeName << " w/ "
                  << G4BestUnit(theTrack->GetKineticEnergy(),"Energy")
                  << " MeV");

    fStenchAndRot = 0;
    fSteps = 0;
    fThrottle = 1000;
}
