#include "kinem/EDepSimVTimeFactory.hh"
#include "kinem/EDepSimFreeTimeFactory.hh"
#include "kinem/EDepSimFreeTimeGenerator.hh"

EDepSim::FreeTimeFactory::FreeTimeFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VTimeFactory("free",parent) { }

EDepSim::FreeTimeFactory::~FreeTimeFactory() { }

EDepSim::VTimeGenerator* EDepSim::FreeTimeFactory::GetGenerator() {
    EDepSim::VTimeGenerator* time = new EDepSim::FreeTimeGenerator(GetName());
    return time;
}
