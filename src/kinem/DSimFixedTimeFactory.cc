#include "kinem/DSimVTimeFactory.hh"
#include "kinem/DSimFixedTimeFactory.hh"
#include "kinem/DSimFixedTimeGenerator.hh"

DSimFixedTimeFactory::DSimFixedTimeFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVTimeFactory("fixed",parent), fTime(1) {

    fTimeCMD = new G4UIcmdWithADoubleAndUnit(CommandName("time"),this);
    fTimeCMD->SetGuidance("Set the time of events to generate.");
    fTimeCMD->SetParameterName("time",false);
    fTimeCMD->SetUnitCategory("Time");

}

DSimFixedTimeFactory::~DSimFixedTimeFactory() {
    delete fTimeCMD;
}

void DSimFixedTimeFactory::SetNewValue(G4UIcommand* command,
                                           G4String newValue) {
    if (command == fTimeCMD) {
        SetTime(fTimeCMD->GetNewDoubleValue(newValue));
    }
}

DSimVTimeGenerator* DSimFixedTimeFactory::GetGenerator() {
    DSimVTimeGenerator* time = new DSimFixedTimeGenerator(GetName(),
                                                            GetTime());
    return time;
}
