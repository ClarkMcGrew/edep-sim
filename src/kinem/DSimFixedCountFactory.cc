#include "kinem/DSimVCountFactory.hh"
#include "kinem/DSimFixedCountFactory.hh"
#include "kinem/DSimFixedCountGenerator.hh"

DSimFixedCountFactory::DSimFixedCountFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVCountFactory("fixed",parent), fNumber(1) {

    fNumberCMD = new G4UIcmdWithAnInteger(CommandName("number"),this);
    fNumberCMD->SetGuidance("Set the number of events to generate.");
    fNumberCMD->SetParameterName("count",false);

}

DSimFixedCountFactory::~DSimFixedCountFactory() {
    delete fNumberCMD;
}

void DSimFixedCountFactory::SetNewValue(G4UIcommand* command,
                                           G4String newValue) {
    if (command == fNumberCMD) {
        SetNumber(fNumberCMD->GetNewIntValue(newValue));
    }
    else {
        DSimVCountFactory::SetNewValue(command,newValue);
    }
}

DSimVCountGenerator* DSimFixedCountFactory::GetGenerator() {
    DSimVCountGenerator* counter 
        = new DSimFixedCountGenerator(GetName(),
                                       GetNumber(),
                                       GetIntensity());
    return counter;
}
