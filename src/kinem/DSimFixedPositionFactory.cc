#include "kinem/DSimVPositionFactory.hh"
#include "kinem/DSimFixedPositionFactory.hh"
#include "kinem/DSimFixedPositionGenerator.hh"

DSimFixedPositionFactory::DSimFixedPositionFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVPositionFactory("fixed",parent),
      fPosition(0,0,0) {

    fPositionCMD = new G4UIcmdWith3VectorAndUnit(CommandName("position"),this);
    fPositionCMD->SetGuidance("Set the position of events to generate.");
    fPositionCMD->SetParameterName("x","y","z",false);
    fPositionCMD->SetUnitCategory("Length");

}

DSimFixedPositionFactory::~DSimFixedPositionFactory() {}

DSimVPositionGenerator* DSimFixedPositionFactory::GetGenerator() {
    DSimVPositionGenerator* vertex 
        = new DSimFixedPositionGenerator(GetName(),GetPosition());
    return vertex;
}

void DSimFixedPositionFactory::SetNewValue(G4UIcommand* command,
                                            G4String newValue) {
    if (command == fPositionCMD) {
        SetPosition(fPositionCMD->GetNew3VectorValue(newValue));
    }
}

