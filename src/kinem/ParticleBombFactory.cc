#include "kinem/ParticleBombFactory.hh"
#include "kinem/ParticleBombGenerator.hh"

EDepSim::ParticleBombFactory::ParticleBombFactory(
    EDepSim::UserPrimaryGeneratorMessenger* parent)
    : EDepSim::VKinematicsFactory("bomb", parent, false),
      _verbose(0),
      _cfg_cmd(nullptr),
      _verbose_cmd(nullptr)
{
  _cfg_cmd = new G4UIcmdWithAString(CommandName("config"), this);
  _cfg_cmd->SetGuidance("Set config file");
  _cfg_cmd->SetParameterName("yaml", false);

  _verbose_cmd = new G4UIcmdWithAnInteger(CommandName("verbose"), this);
  _verbose_cmd->SetGuidance("Set verbosity level (default: 0)");
  _verbose_cmd->SetParameterName("number", false, true);
}

EDepSim::ParticleBombFactory::~ParticleBombFactory()
{
  if (!_cfg_cmd) delete _cfg_cmd;
  if (!_verbose_cmd) delete _verbose_cmd;
}

EDepSim::VKinematicsGenerator*
EDepSim::ParticleBombFactory::GetGenerator() 
{
  return new EDepSim::ParticleBombGenerator(GetName(), _cfg_file, _verbose > 0);
}

void EDepSim::ParticleBombFactory::SetNewValue(
    G4UIcommand* cmd, G4String value) 
{

    if (cmd == _cfg_cmd) {
      _cfg_file = value;
    }
    else if (cmd == _verbose_cmd) {
      _verbose = _verbose_cmd->GetNewIntValue(value);
    }
    else{
        EDepSimError("Invalid command");
        EDepSimThrow("EDepSim::ParticleBombFactory::SetNewValue(): Error");
    }
}
