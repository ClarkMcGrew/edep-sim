////////////////////////////////////////////////////////////
// $Id: DSimUserStackingAction.cc,v 1.4 2011/09/06 18:58:35 mcgrew Exp $
//

#include <globals.hh>
#include <G4ParticleDefinition.hh>
// #include <G4VProcess.hh>

#include "DSimUserStackingAction.hh"

DSimUserStackingAction::DSimUserStackingAction() { }

DSimUserStackingAction::~DSimUserStackingAction() { }

G4ClassificationOfNewTrack
DSimUserStackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    
    if (aTrack->GetCurrentStepNumber() == 0) {
        // Get the particle type of the new track.
        const G4ParticleDefinition* particle = aTrack->GetDefinition();
        
        // This is where we can throw away particles that we don't want to
        // track.
        if (particle->GetParticleName() == "gamma") {
            if (aTrack->GetKineticEnergy() < 10.*keV) return fKill;
        }
    }

    return fUrgent;
}
