#include "kinem/DSimVCountFactory.hh"
#include "kinem/DSimMeanCountFactory.hh"
#include "kinem/DSimMeanCountGenerator.hh"

DSimMeanCountFactory::DSimMeanCountFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVCountFactory("mean",parent), fNumber(1.0) {

    fNumberCMD = new G4UIcmdWithADouble(CommandName("number"),this);
    fNumberCMD->SetGuidance("Set the mean number of events to generate.");
    fNumberCMD->SetParameterName("mean",false);

}

DSimMeanCountFactory::~DSimMeanCountFactory() {
    delete fNumberCMD;
}

void DSimMeanCountFactory::SetNewValue(G4UIcommand* command,
                                           G4String newValue) {
    if (command == fNumberCMD) {
        SetNumber(fNumberCMD->GetNewDoubleValue(newValue));
    }
    else {
        DSimVCountFactory::SetNewValue(command,newValue);
    }
}

DSimVCountGenerator* DSimMeanCountFactory::GetGenerator() {
    DSimVCountGenerator* counter = new DSimMeanCountGenerator(GetName(),
                                                                GetNumber(),
                                                                GetIntensity());
    return counter;
}

