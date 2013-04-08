////////////////////////////////////////////////////////////
// $Id: DSimUserSteppingAction.cc,v 1.11 2008/08/21 18:36:54 mcgrew Exp $
//

#include "TCaptLog.hxx"

#include "DSimUserSteppingAction.hh"
#include <G4UnitsTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4StepPoint.hh>
#include <G4Track.hh>
#include <G4Step.hh>

DSimUserSteppingAction::DSimUserSteppingAction()
    : fStenchAndRot(0) {}

void DSimUserSteppingAction::UserSteppingAction(const G4Step* theStep) { 

    G4Track* theTrack = theStep->GetTrack();

    G4String theParticleName = theTrack->GetDefinition()->GetParticleName();
    G4String theCurrentVolumeName = theStep->GetPreStepPoint()->
                                             GetPhysicalVolume()->GetName();

    static int steps = 0;
    static int throttle = 1000;
    static int governor = 0;
    if (steps%throttle == 0 && steps>0) {
        CaptWarn("DSimUserSteppingAction:: Excessive Steps "
                  << " " << steps 
                  << " " << theParticleName
                  << " " << theTrack->GetTrackLength()/m << " m" 
                  << " " << theTrack->GetGlobalTime()/ns << " ns"
                  << " " << theTrack->GetTotalEnergy()/MeV << " MeV"
                  << " " << theCurrentVolumeName);
        throttle *= 2;
        governor = 5;
    }
    if (governor>0) {
        --governor;
        CaptDebug("    " << theCurrentVolumeName
                   << ": " << theParticleName
                   << " -- Step: " << theStep->GetStepLength()/mm << " mm"
                   << " Energy Loss: "<< theStep->GetDeltaEnergy()/MeV 
                   << " MeV");
    }

    if (theStep->GetTrack()->GetTrackLength() > 100*m) {
        theTrack->SetTrackStatus(fStopAndKill);
        CaptSevere("Stop and kill w/ track too long for " 
                    << theParticleName << " w/ " 
                    << G4BestUnit(theTrack->GetKineticEnergy(),"Energy")
                    << " (" << G4BestUnit(theTrack->GetTrackLength(),
                                          "Length")
                    << ")");
        fStenchAndRot = 0;
        steps = 0;
        throttle = 1000;
        return;
    }

    ++steps;
    if (steps>50000) {
        theTrack->SetTrackStatus(fStopAndKill);
        CaptSevere("Stop and kill with to many steps for " 
                    << theParticleName << " in "
                    << theCurrentVolumeName << " w/ " 
                    << G4BestUnit(theTrack->GetKineticEnergy(),"Energy"));
        fStenchAndRot = 0;
        steps = 0;
        throttle = 1000;
        return;
    }

    if (theTrack->GetTrackStatus()==fStopAndKill) {
        fStenchAndRot = 0;
        steps = 0;
        throttle = 1000;
        return;
    }

    // Make sure the particle is someplace near the detector
    if (theTrack->GetPosition().mag()>50.0*meter) {
        theTrack->SetTrackStatus(fStopAndKill);
        CaptSevere("Stop and kill track far from detector");
        fStenchAndRot = 0;
        return;
    }

    if (theStep->GetStepLength()>1*nanometer) {
        fStenchAndRot = 0;
        return;
    }

    if (theStep->GetDeltaTime()>0.1*picosecond) {
        fStenchAndRot = 0;
        return;
    }

    ++fStenchAndRot;
    if (fStenchAndRot>10) {
        theTrack->SetTrackStatus(fStopAndKill);
        CaptSevere("Stop and kill stuck " 
                    << theParticleName << " in "
                    << theCurrentVolumeName << " w/ " 
                    << G4BestUnit(theTrack->GetKineticEnergy(),"Energy")
                    << " MeV");
        fStenchAndRot = 0;
        steps = 0;
        throttle = 1000;
    }
}
