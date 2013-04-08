#include "kinem/DSimVPositionFactory.hh"
#include "kinem/DSimFreePositionFactory.hh"
#include "kinem/DSimFreePositionGenerator.hh"

DSimFreePositionFactory::DSimFreePositionFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVPositionFactory("free",parent,false) {
}

DSimFreePositionFactory::~DSimFreePositionFactory() {
}

DSimVPositionGenerator* DSimFreePositionFactory::GetGenerator() {
    DSimVPositionGenerator* vertex = new DSimFreePositionGenerator(GetName());
    return vertex;
}
