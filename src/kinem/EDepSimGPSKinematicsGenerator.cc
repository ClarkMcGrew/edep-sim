#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>

#include "kinem/EDepSimGPSKinematicsGenerator.hh"

EDepSim::GPSKinematicsGenerator::GPSKinematicsGenerator(
    const G4String& name, G4VPrimaryGenerator* gen) 
    : EDepSim::VKinematicsGenerator(name), fGenerator(gen) {}

EDepSim::GPSKinematicsGenerator::~GPSKinematicsGenerator() {}

bool EDepSim::GPSKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* evt, G4LorentzVector& position) {
    fGenerator->GeneratePrimaryVertex(evt);
    return true;
}
