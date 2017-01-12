#include "kinem/EDepSimVKinematicsFactory.hh"

EDepSim::VKinematicsFactory::VKinematicsFactory(
    G4String name,
    EDepSim::UserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : EDepSim::VPrimaryFactory("kinematics",name,parent,makeDirectory) {}

EDepSim::VKinematicsFactory::~VKinematicsFactory() {}

