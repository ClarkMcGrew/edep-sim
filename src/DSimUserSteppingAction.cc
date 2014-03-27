////////////////////////////////////////////////////////////
// $Id: DSimUserSteppingAction.cc,v 1.11 2008/08/21 18:36:54 mcgrew Exp $
//

#include "DSimLog.hh"

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

#ifdef KILL_NEUTRONS
    if (theParticleName == "neutron") {
        if (theTrack->GetKineticEnergy() < 1.*MeV) {
            DSimSevere("DSimUserSteppingAction:: Kill " << theParticleName
                      << " " << theTrack->GetCurrentStepNumber()
                      << " " << theTrack->GetTrackLength()/m << " m" 
                      << " " << theTrack->GetGlobalTime()/ns << " ns"
                      << " " << theTrack->GetTotalEnergy()/MeV << " MeV"
                      << " " << theCurrentVolumeName);
            theTrack->SetTrackStatus(fStopAndKill);
        }
    }
#endif

    const int defaultThrottle = 10000;
    static int steps = 0;
    static int throttle = defaultThrottle;
    static int governor = 0;
    if (steps%throttle == 0 && steps>0) {
        DSimSevere("DSimUserSteppingAction:: Excessive Steps "
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
        DSimDebug("    " << theCurrentVolumeName
                   << ": " << theParticleName
                   << " -- Step: " << theStep->GetStepLength()/mm << " mm"
                   << " Energy Loss: "<< theStep->GetDeltaEnergy()/MeV 
                   << " MeV");
    }

    if (theStep->GetTrack()->GetTrackLength() > 100*m) {
        theTrack->SetTrackStatus(fStopAndKill);
        DSimSevere("Stop and kill w/ track too long for " 
                    << theParticleName << " w/ " 
                    << G4BestUnit(theTrack->GetKineticEnergy(),"Energy")
                    << " (" << G4BestUnit(theTrack->GetTrackLength(),
                                          "Length")
                    << ")");
        fStenchAndRot = 0;
        steps = 0;
        throttle = defaultThrottle;
        return;
    }

    ++steps;
    if (steps>100000) {
        theTrack->SetTrackStatus(fStopAndKill);
        DSimSevere("Stop and kill with to many steps for " 
                  << theParticleName << " in "
                  << theCurrentVolumeName << " w/ " 
                  << G4BestUnit(theTrack->GetKineticEnergy(),"Energy"));
        fStenchAndRot = 0;
        steps = 0;
        throttle = defaultThrottle;
        return;
    }

    if (theTrack->GetTrackStatus()==fStopAndKill) {
        fStenchAndRot = 0;
        steps = 0;
        throttle = defaultThrottle;
        return;
    }

    // Make sure the particle is someplace near the detector
    if (theTrack->GetPosition().mag()>2000.0*meter) {
        theTrack->SetTrackStatus(fStopAndKill);
        DSimError("Stop and kill track far from detector");
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
        DSimError("Stop and kill stuck " 
                    << theParticleName << " in "
                    << theCurrentVolumeName << " w/ " 
                    << G4BestUnit(theTrack->GetKineticEnergy(),"Energy")
                    << " MeV");
        fStenchAndRot = 0;
        steps = 0;
        throttle = defaultThrottle;
    }
}
