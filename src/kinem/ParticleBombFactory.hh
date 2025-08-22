#ifndef EDepSim_ParticleBombFactory_hh
#define EDepSim_ParticleBombFactory_hh

#include "kinem/EDepSimVKinematicsFactory.hh"

class G4VPrimaryGenerator;

namespace EDepSim
{
  class ParticleBombFactory : public VKinematicsFactory 
  {
    public:
      ParticleBombFactory(EDepSim::UserPrimaryGeneratorMessenger* fParent);
      ~ParticleBombFactory();

      EDepSim::VKinematicsGenerator* GetGenerator();

      void SetNewValue(G4UIcommand* , G4String);

    private:
      G4String _cfg_file;
      int _verbose;

      G4UIcmdWithAString *_cfg_cmd;
      G4UIcmdWithAnInteger *_verbose_cmd;
  };
}
#endif
