#include "kinem/EDepSimVPositionFactory.hh"
#include "kinem/EDepSimFixedPositionFactory.hh"
#include "kinem/EDepSimFixedPositionGenerator.hh"

EDepSim::FixedPositionFactory::FixedPositionFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VPositionFactory("fixed",parent),
      fPosition(0,0,0) {

    fPositionCMD = new G4UIcmdWith3VectorAndUnit(CommandName("position"),this);
    fPositionCMD->SetGuidance("Set the position of events to generate.");
    fPositionCMD->SetParameterName("x","y","z",false);
    fPositionCMD->SetUnitCategory("Length");

}

EDepSim::FixedPositionFactory::~FixedPositionFactory() {}

EDepSim::VPositionGenerator* EDepSim::FixedPositionFactory::GetGenerator() {
    EDepSim::VPositionGenerator* vertex 
        = new EDepSim::FixedPositionGenerator(GetName(),GetPosition());
    return vertex;
}

void EDepSim::FixedPositionFactory::SetNewValue(G4UIcommand* command,
                                            G4String newValue) {
    if (command == fPositionCMD) {
        SetPosition(fPositionCMD->GetNew3VectorValue(newValue));
    }
}

