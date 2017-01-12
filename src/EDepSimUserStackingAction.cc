////////////////////////////////////////////////////////////
// $Id: EDepSim::UserStackingAction.cc,v 1.4 2011/09/06 18:58:35 mcgrew Exp $
//

#include "EDepSimUserStackingAction.hh"
#include "EDepSimLog.hh"

#include <globals.hh>
#include <G4ParticleDefinition.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

EDepSim::UserStackingAction::UserStackingAction() { }

EDepSim::UserStackingAction::~UserStackingAction() { }

G4ClassificationOfNewTrack
EDepSim::UserStackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    // Get the particle type of the new track.
    const G4ParticleDefinition* particle = aTrack->GetDefinition();
    
    // if (aTrack->GetCurrentStepNumber() > 0) return fUrgent;

    if (aTrack->GetParentID() <= 0) return fUrgent;

    // This is where we can throw away particles that we don't want to track.
    // Drop photons below the "lowest" nuclear lines.  The lowest I know if is
    // about 6 keV, and atomic shells start messing with the cross section at
    // about 70 keV.
    if (particle->GetParticleName() == "gamma") {
        if (aTrack->GetKineticEnergy() < 10.*CLHEP::keV) return fKill;
    }

    if (particle->GetParticleName() == "opticalphoton") {
        return fKill;
    }

    if (particle->GetParticleName() == "thermalelectron") {
        return fKill;
    }

    return G4UserStackingAction::ClassifyNewTrack(aTrack);
}
