#include "kinem/DSimVTimeFactory.hh"

DSimVTimeFactory::DSimVTimeFactory(
    G4String name,
    DSimUserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : DSimVPrimaryFactory("time",name,parent,makeDirectory) {}

DSimVTimeFactory::~DSimVTimeFactory() {}

