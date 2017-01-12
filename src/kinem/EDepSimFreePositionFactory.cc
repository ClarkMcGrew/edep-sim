#include "kinem/EDepSimVPositionFactory.hh"
#include "kinem/EDepSimFreePositionFactory.hh"
#include "kinem/EDepSimFreePositionGenerator.hh"

EDepSim::FreePositionFactory::FreePositionFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VPositionFactory("free",parent,false) {
}

EDepSim::FreePositionFactory::~FreePositionFactory() {
}

EDepSim::VPositionGenerator* EDepSim::FreePositionFactory::GetGenerator() {
    EDepSim::VPositionGenerator* vertex = new EDepSim::FreePositionGenerator(GetName());
    return vertex;
}
