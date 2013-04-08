#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>

#include "kinem/DSimGPSKinematicsGenerator.hh"

DSimGPSKinematicsGenerator::DSimGPSKinematicsGenerator(
    const G4String& name, G4VPrimaryGenerator* gen) 
    : DSimVKinematicsGenerator(name), fGenerator(gen) {}

DSimGPSKinematicsGenerator::~DSimGPSKinematicsGenerator() {}

bool DSimGPSKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* evt, G4LorentzVector& position) {
    fGenerator->GeneratePrimaryVertex(evt);
    return true;
}
