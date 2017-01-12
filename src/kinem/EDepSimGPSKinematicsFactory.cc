#include <G4GeneralParticleSource.hh>

#include "kinem/EDepSimGPSKinematicsFactory.hh"
#include "kinem/EDepSimGPSKinematicsGenerator.hh"

EDepSim::GPSKinematicsFactory::GPSKinematicsFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VKinematicsFactory("gps",parent,false) {
    fGenerator = new G4GeneralParticleSource();
}

EDepSim::GPSKinematicsFactory::~GPSKinematicsFactory() {
    delete fGenerator;
}

EDepSim::VKinematicsGenerator* EDepSim::GPSKinematicsFactory::GetGenerator() {
    EDepSim::VKinematicsGenerator* kine
        = new EDepSim::GPSKinematicsGenerator(GetName(), fGenerator);
    return kine;
}
