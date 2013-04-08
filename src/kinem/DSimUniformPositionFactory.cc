#include "kinem/DSimVPositionFactory.hh"
#include "kinem/DSimUniformPositionFactory.hh"
#include "kinem/DSimUniformPositionGenerator.hh"

DSimUniformPositionFactory::DSimUniformPositionFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVConstrainedPositionFactory("uniform",parent) {
}

DSimUniformPositionFactory::~DSimUniformPositionFactory() {
}

DSimVPositionGenerator* DSimUniformPositionFactory::CreateGenerator() {
    DSimVPositionGenerator* vtx
        = new DSimUniformPositionGenerator(GetName());
    return vtx;
}

DSimVPositionGenerator* DSimUniformPositionFactory::GetGenerator() {
    return DSimVConstrainedPositionFactory::GetGenerator();
}

void DSimUniformPositionFactory::SetNewValue(G4UIcommand* command,
                                            G4String value) {
    DSimVConstrainedPositionFactory::SetNewValue(command,value);
}
