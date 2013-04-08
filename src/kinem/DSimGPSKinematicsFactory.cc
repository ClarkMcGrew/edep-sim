#include <G4GeneralParticleSource.hh>

#include "kinem/DSimGPSKinematicsFactory.hh"
#include "kinem/DSimGPSKinematicsGenerator.hh"

DSimGPSKinematicsFactory::DSimGPSKinematicsFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVKinematicsFactory("gps",parent,false) {
    fGenerator = new G4GeneralParticleSource();
}

DSimGPSKinematicsFactory::~DSimGPSKinematicsFactory() {
    delete fGenerator;
}

DSimVKinematicsGenerator* DSimGPSKinematicsFactory::GetGenerator() {
    DSimVKinematicsGenerator* kine
        = new DSimGPSKinematicsGenerator(GetName(), fGenerator);
    return kine;
}
