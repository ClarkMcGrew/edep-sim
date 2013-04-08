#include "kinem/DSimVKinematicsFactory.hh"

DSimVKinematicsFactory::DSimVKinematicsFactory(
    G4String name,
    DSimUserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : DSimVPrimaryFactory("kinematics",name,parent,makeDirectory) {}

DSimVKinematicsFactory::~DSimVKinematicsFactory() {}

