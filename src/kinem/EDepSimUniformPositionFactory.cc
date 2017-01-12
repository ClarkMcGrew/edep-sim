#include "kinem/EDepSimVPositionFactory.hh"
#include "kinem/EDepSimUniformPositionFactory.hh"
#include "kinem/EDepSimUniformPositionGenerator.hh"

EDepSim::UniformPositionFactory::UniformPositionFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VConstrainedPositionFactory("uniform",parent) {
}

EDepSim::UniformPositionFactory::~UniformPositionFactory() {
}

EDepSim::VPositionGenerator* EDepSim::UniformPositionFactory::CreateGenerator() {
    EDepSim::VPositionGenerator* vtx
        = new EDepSim::UniformPositionGenerator(GetName());
    return vtx;
}

EDepSim::VPositionGenerator* EDepSim::UniformPositionFactory::GetGenerator() {
    return EDepSim::VConstrainedPositionFactory::GetGenerator();
}

void EDepSim::UniformPositionFactory::SetNewValue(G4UIcommand* command,
                                            G4String value) {
    EDepSim::VConstrainedPositionFactory::SetNewValue(command,value);
}
