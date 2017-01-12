#include "kinem/EDepSimNuMIRockKinematicsFactory.hh"
#include "kinem/EDepSimNuMIRockKinematicsGenerator.hh"

EDepSim::NuMIRockKinematicsFactory::NuMIRockKinematicsFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VKinematicsFactory("rock",parent,false) {}

EDepSim::NuMIRockKinematicsFactory::~NuMIRockKinematicsFactory() {}

EDepSim::VKinematicsGenerator* EDepSim::NuMIRockKinematicsFactory::GetGenerator() {
    EDepSim::VKinematicsGenerator* kine
        = new EDepSim::NuMIRockKinematicsGenerator(GetName(), "medium");
    return kine;
}
