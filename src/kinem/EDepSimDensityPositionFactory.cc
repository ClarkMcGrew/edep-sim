#include "kinem/EDepSimVPositionFactory.hh"
#include "kinem/EDepSimDensityPositionFactory.hh"
#include "kinem/EDepSimDensityPositionGenerator.hh"

EDepSim::DensityPositionFactory::DensityPositionFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VConstrainedPositionFactory("density",parent) {
}

EDepSim::DensityPositionFactory::~DensityPositionFactory() {
}

EDepSim::VPositionGenerator* EDepSim::DensityPositionFactory::CreateGenerator() {
    EDepSim::VPositionGenerator* vtx
        = new EDepSim::DensityPositionGenerator(GetName());
    return vtx;
}

EDepSim::VPositionGenerator* EDepSim::DensityPositionFactory::GetGenerator() {
    return EDepSim::VConstrainedPositionFactory::GetGenerator();
}

void EDepSim::DensityPositionFactory::SetNewValue(G4UIcommand* command,
                                            G4String value) {
    EDepSim::VConstrainedPositionFactory::SetNewValue(command,value);
}
