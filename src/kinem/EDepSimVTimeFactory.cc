#include "kinem/EDepSimVTimeFactory.hh"

EDepSim::VTimeFactory::VTimeFactory(
    G4String name,
    EDepSim::UserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : EDepSim::VPrimaryFactory("time",name,parent,makeDirectory) {}

EDepSim::VTimeFactory::~VTimeFactory() {}

