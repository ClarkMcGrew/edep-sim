#include "kinem/EDepSimVCountFactory.hh"
#include "kinem/EDepSimMeanCountFactory.hh"
#include "kinem/EDepSimMeanCountGenerator.hh"

EDepSim::MeanCountFactory::MeanCountFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VCountFactory("mean",parent), fNumber(1.0) {

    fNumberCMD = new G4UIcmdWithADouble(CommandName("number"),this);
    fNumberCMD->SetGuidance("Set the mean number of events to generate.");
    fNumberCMD->SetParameterName("mean",false);

}

EDepSim::MeanCountFactory::~MeanCountFactory() {
    delete fNumberCMD;
}

void EDepSim::MeanCountFactory::SetNewValue(G4UIcommand* command,
                                           G4String newValue) {
    if (command == fNumberCMD) {
        SetNumber(fNumberCMD->GetNewDoubleValue(newValue));
    }
    else {
        EDepSim::VCountFactory::SetNewValue(command,newValue);
    }
}

EDepSim::VCountGenerator* EDepSim::MeanCountFactory::GetGenerator() {
    EDepSim::VCountGenerator* counter = new EDepSim::MeanCountGenerator(GetName(),
                                                                GetNumber(),
                                                                GetIntensity());
    return counter;
}

