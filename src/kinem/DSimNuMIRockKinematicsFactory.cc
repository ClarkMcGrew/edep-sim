#include "kinem/DSimNuMIRockKinematicsFactory.hh"
#include "kinem/DSimNuMIRockKinematicsGenerator.hh"

DSimNuMIRockKinematicsFactory::DSimNuMIRockKinematicsFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVKinematicsFactory("rock",parent,false) {}

DSimNuMIRockKinematicsFactory::~DSimNuMIRockKinematicsFactory() {}

DSimVKinematicsGenerator* DSimNuMIRockKinematicsFactory::GetGenerator() {
    DSimVKinematicsGenerator* kine
        = new DSimNuMIRockKinematicsGenerator(GetName(), "medium");
    return kine;
}
