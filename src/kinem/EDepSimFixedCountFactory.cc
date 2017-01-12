#include "kinem/EDepSimVCountFactory.hh"
#include "kinem/EDepSimFixedCountFactory.hh"
#include "kinem/EDepSimFixedCountGenerator.hh"

EDepSim::FixedCountFactory::FixedCountFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VCountFactory("fixed",parent), fNumber(1) {

    fNumberCMD = new G4UIcmdWithAnInteger(CommandName("number"),this);
    fNumberCMD->SetGuidance("Set the number of events to generate.");
    fNumberCMD->SetParameterName("count",false);

}

EDepSim::FixedCountFactory::~FixedCountFactory() {
    delete fNumberCMD;
}

void EDepSim::FixedCountFactory::SetNewValue(G4UIcommand* command,
                                           G4String newValue) {
    if (command == fNumberCMD) {
        SetNumber(fNumberCMD->GetNewIntValue(newValue));
    }
    else {
        EDepSim::VCountFactory::SetNewValue(command,newValue);
    }
}

EDepSim::VCountGenerator* EDepSim::FixedCountFactory::GetGenerator() {
    EDepSim::VCountGenerator* counter 
        = new EDepSim::FixedCountGenerator(GetName(),
                                       GetNumber(),
                                       GetIntensity());
    return counter;
}
