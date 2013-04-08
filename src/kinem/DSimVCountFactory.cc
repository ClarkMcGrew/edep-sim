#include "kinem/DSimVCountFactory.hh"

DSimVCountFactory::DSimVCountFactory(
    G4String name,
    DSimUserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : DSimVPrimaryFactory("count",name,parent,makeDirectory), fIntensity(0.0)
{
    fIntensityCMD = new G4UIcmdWithADouble(CommandName("intensity"),this);
    fIntensityCMD->SetGuidance("Set the intensity for the events.  This is"
                               " only for documentation and is saved in the"
                               " MC header.");
    fIntensityCMD->SetParameterName("intensity",false);
}

DSimVCountFactory::~DSimVCountFactory() {
    delete fIntensityCMD;
}

void DSimVCountFactory::SetNewValue(G4UIcommand* command,
                                     G4String newValue) {
    if (command == fIntensityCMD) {
        SetIntensity(fIntensityCMD->GetNewDoubleValue(newValue));
    }
}
