////////////////////////////////////////////////////////////
// $Id: DSimUserStackingAction.cc,v 1.4 2011/09/06 18:58:35 mcgrew Exp $
//

#include "DSimUserStackingAction.hh"
#include "DSimLog.hh"

#include <globals.hh>
#include <G4ParticleDefinition.hh>

DSimUserStackingAction::DSimUserStackingAction() { }

DSimUserStackingAction::~DSimUserStackingAction() { }

G4ClassificationOfNewTrack
DSimUserStackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    // Get the particle type of the new track.
    const G4ParticleDefinition* particle = aTrack->GetDefinition();
    
    // if (aTrack->GetCurrentStepNumber() > 0) return fUrgent;

    if (aTrack->GetParentID() <= 0) return fUrgent;

    // This is where we can throw away particles that we don't want to
    // track.
    if (particle->GetParticleName() == "gamma") {
        if (aTrack->GetKineticEnergy() < 5.*keV) return fKill;
    }

    if (particle->GetParticleName() == "opticalphoton") {
        return fKill;
    }

    if (particle->GetParticleName() == "thermalelectron") {
        return fKill;
    }

    return G4UserStackingAction::ClassifyNewTrack(aTrack);
}
