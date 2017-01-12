#include "kinem/EDepSimVCountFactory.hh"

EDepSim::VCountFactory::VCountFactory(
    G4String name,
    EDepSim::UserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : EDepSim::VPrimaryFactory("count",name,parent,makeDirectory), fIntensity(0.0)
{
    fIntensityCMD = new G4UIcmdWithADouble(CommandName("intensity"),this);
    fIntensityCMD->SetGuidance("Set the intensity for the events.  This is"
                               " only for documentation and is saved in the"
                               " MC header.");
    fIntensityCMD->SetParameterName("intensity",false);
}

EDepSim::VCountFactory::~VCountFactory() {
    delete fIntensityCMD;
}

void EDepSim::VCountFactory::SetNewValue(G4UIcommand* command,
                                     G4String newValue) {
    if (command == fIntensityCMD) {
        SetIntensity(fIntensityCMD->GetNewDoubleValue(newValue));
    }
}
