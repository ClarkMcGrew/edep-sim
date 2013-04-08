#include "kinem/DSimVPositionFactory.hh"
#include "kinem/DSimDensityPositionFactory.hh"
#include "kinem/DSimDensityPositionGenerator.hh"

DSimDensityPositionFactory::DSimDensityPositionFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVConstrainedPositionFactory("density",parent) {
}

DSimDensityPositionFactory::~DSimDensityPositionFactory() {
}

DSimVPositionGenerator* DSimDensityPositionFactory::CreateGenerator() {
    DSimVPositionGenerator* vtx
        = new DSimDensityPositionGenerator(GetName());
    return vtx;
}

DSimVPositionGenerator* DSimDensityPositionFactory::GetGenerator() {
    return DSimVConstrainedPositionFactory::GetGenerator();
}

void DSimDensityPositionFactory::SetNewValue(G4UIcommand* command,
                                            G4String value) {
    DSimVConstrainedPositionFactory::SetNewValue(command,value);
}
