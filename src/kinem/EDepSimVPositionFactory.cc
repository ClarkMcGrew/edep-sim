#include "kinem/EDepSimVPositionFactory.hh"

EDepSim::VPositionFactory::VPositionFactory(
    G4String name,
    EDepSim::UserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : EDepSim::VPrimaryFactory("position",name,parent,makeDirectory) {}

EDepSim::VPositionFactory::~VPositionFactory() {}

