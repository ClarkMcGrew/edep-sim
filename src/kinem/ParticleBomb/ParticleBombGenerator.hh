#ifndef EDepSim_ParticleBombGenerator_hh
#define EDepSim_ParticleBombGenerator_hh

#include <string>
#include <fstream>

#include <globals.hh>
#include <G4LorentzVector.hh>

#include "kinem/EDepSimVKinematicsGenerator.hh"

#include "yaml-cpp/yaml.h"
#include "DLPGenerator/ParticleBomb/ParticleBomb.h"


class G4Event;
class G4VPrimaryGenerator;

namespace EDepSim
{
  class ParticleBombGenerator : public VKinematicsGenerator 
  {
    public:
      ParticleBombGenerator(const G4String&, const G4String&, bool); 
      ~ParticleBombGenerator();
      
      GeneratorStatus
      GeneratePrimaryVertex(G4Event*, const G4LorentzVector&);

    private:
      DLPGenerator::ParticleBomb _generator;
      DLPGenerator::GenParamParticle _parse_particle(const YAML::Node&);
      DLPGenerator::GenParamInteraction _parse_interaction(const YAML::Node&);
  };
}
#endif
