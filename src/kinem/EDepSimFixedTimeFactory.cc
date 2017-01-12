#include "kinem/EDepSimVTimeFactory.hh"
#include "kinem/EDepSimFixedTimeFactory.hh"
#include "kinem/EDepSimFixedTimeGenerator.hh"

EDepSim::FixedTimeFactory::FixedTimeFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent) 
    : EDepSim::VTimeFactory("fixed",parent), fTime(1) {

    fTimeCMD = new G4UIcmdWithADoubleAndUnit(CommandName("time"),this);
    fTimeCMD->SetGuidance("Set the time of events to generate.");
    fTimeCMD->SetParameterName("time",false);
    fTimeCMD->SetUnitCategory("Time");

}

EDepSim::FixedTimeFactory::~FixedTimeFactory() {
    delete fTimeCMD;
}

void EDepSim::FixedTimeFactory::SetNewValue(G4UIcommand* command,
                                           G4String newValue) {
    if (command == fTimeCMD) {
        SetTime(fTimeCMD->GetNewDoubleValue(newValue));
    }
}

EDepSim::VTimeGenerator* EDepSim::FixedTimeFactory::GetGenerator() {
    EDepSim::VTimeGenerator* time = new EDepSim::FixedTimeGenerator(GetName(),
                                                            GetTime());
    return time;
}
