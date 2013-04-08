#include "kinem/DSimVPositionFactory.hh"

DSimVPositionFactory::DSimVPositionFactory(
    G4String name,
    DSimUserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : DSimVPrimaryFactory("position",name,parent,makeDirectory) {}

DSimVPositionFactory::~DSimVPositionFactory() {}

