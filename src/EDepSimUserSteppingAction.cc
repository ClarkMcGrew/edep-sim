#include "EDepSimUserSteppingAction.hh"
#include "EDepSimLog.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4StepPoint.hh>
#include <G4Track.hh>
#include <G4Step.hh>

EDepSim::SteppingAction::SteppingAction()
    : fStenchAndRot(0), fSteps(0), fThrottle(1000), fGovernor(0) {}

void EDepSim::SteppingAction::UserSteppingAction(const G4Step* theStep) {

    G4Track* theTrack = theStep->GetTrack();

    G4String theParticleName = theTrack->GetDefinition()->GetParticleName();
    G4String theCurrentVolumeName = theStep->GetPreStepPoint()->
                                             GetPhysicalVolume()->GetName();

    if (fSteps%fThrottle == 0 && fSteps>0) {
        EDepSimWarn("EDepSimUserSteppingAction:: Excessive Steps "
                  << " " << fSteps
                  << " " << theParticleName
                  << " " << theTrack->GetTrackLength()/m << " m"
                  << " " << theTrack->GetGlobalTime()/ns << " ns"
                  << " " << theTrack->GetTotalEnergy()/MeV << " MeV"
                  << " " << theCurrentVolumeName);
        fThrottle *= 2;
        fGovernor = 5;
    }
    if (fGovernor>0) {
        // Print a few steps to help the user see what is happening.
        --fGovernor;
        EDepSimDebug("    " << theCurrentVolumeName
                   << ": " << theParticleName
                   << " -- Step: " << theStep->GetStepLength()/mm << " mm"
                   << " Energy Loss: "<< theStep->GetDeltaEnergy()/MeV
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
