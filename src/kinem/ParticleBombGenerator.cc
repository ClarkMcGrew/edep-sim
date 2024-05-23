#include "kinem/ParticleBombGenerator.hh"
#include "EDepSimException.hh"

#include <G4Event.hh>
#include <G4SystemOfUnits.hh>
#include <G4LorentzVector.hh>
#include <G4PrimaryVertex.hh>

EDepSim::ParticleBombGenerator::ParticleBombGenerator(
    const G4String &name, const G4String &cfg_file, bool verbose) 
  : EDepSim::VKinematicsGenerator(name),
    _generator(0)
{
  auto cfg = YAML::LoadFile(cfg_file);
  int seed = cfg["SEED"].as<int>(-1);
  verbose  = cfg["Debug"].as<bool>(verbose);

  _generator.Seed(seed);
  _generator.Debug(verbose);
  
  for (auto const &item: cfg) {
    if (item.first.as<std::string>() == "SEED" ) continue;
    if (item.first.as<std::string>() == "Debug") continue;

    auto pars = _parse_interaction(item.second);
    
    if(_generator.Add(pars)) {
      std::cerr << "[ParticleBombGenerator] error detected while configuring a generator" << std::endl;
      throw std::exception();
    }
  }
}

EDepSim::ParticleBombGenerator::~ParticleBombGenerator() {
}

EDepSim::VKinematicsGenerator::GeneratorStatus
EDepSim::ParticleBombGenerator::GeneratePrimaryVertex(
    G4Event *evt, const G4LorentzVector& /* position */) {

  if (!(_generator.Configured())) {
      std::cerr << "[ParticleBombGenerator] the generator has not been configured" << std::endl;
      throw std::exception();
  }
  auto bombs = _generator.Generate();
  for (auto const &bomb : bombs) {

    G4PrimaryVertex *vertex = nullptr;
    for (auto const &p : bomb) {

      /* TODO(2021-10-26 kvt) how to handle virtual parent? */
      if (p.status_code == 3) continue;

      if (!vertex) 
        vertex = new G4PrimaryVertex(p.x*mm, p.y*mm, p.z*mm, p.t*ns);

      vertex->SetPrimary(
          new G4PrimaryParticle(p.pdg_code, p.px*GeV, p.py*GeV, p.pz*GeV)
      );
    }
    evt->AddPrimaryVertex(vertex);
  }
  return kEndEvent;
}

DLPGenerator::GenParamParticle 
EDepSim::ParticleBombGenerator::_parse_particle(const YAML::Node &node) 
{
  DLPGenerator::GenParamParticle pars;
  for (auto const &pdg : node["PDG"])
    pars.pdg.push_back(pdg.as<int>());

  pars.multi[0] = node["NumRange"][0].as<size_t>();
  pars.multi[1] = node["NumRange"][1].as<size_t>();

  pars.kerange[0] = node["KERange"][0].as<double>();
  pars.kerange[1] = node["KERange"][1].as<double>();

  if(node["PhiRange"]) {
    pars.phi_range[0] = node["PhiRange"][0].as<double>(0.);
    pars.phi_range[1] = node["PhiRange"][1].as<double>(2*M_PI);
  }

  if(node["ThetaRange"]) {
    pars.theta_range[0] = node["ThetaRange"][0].as<double>(0.);
    pars.theta_range[1] = node["ThetaRange"][1].as<double>(M_PI);
  }

  pars.use_mom = node["UseMom"].as<bool>(false);
  pars.weight = node["Weight"].as<double>(1.);

  return pars;
}

DLPGenerator::GenParamInteraction 
EDepSim::ParticleBombGenerator::_parse_interaction(const YAML::Node &node) 
{

  DLPGenerator::GenParamInteraction pars;

  pars.num_event[0] = node["NumEvent"][0].as<int>();
  pars.num_event[1] = node["NumEvent"][1].as<int>();

  pars.num_particle[0] = node["NumParticle"][0].as<int>();
  pars.num_particle[1] = node["NumParticle"][1].as<int>();

  pars.xrange[0] = node["XRange"][0].as<double>();
  pars.xrange[1] = node["XRange"][1].as<double>();

  pars.yrange[0] = node["YRange"][0].as<double>();
  pars.yrange[1] = node["YRange"][1].as<double>();

  pars.zrange[0] = node["ZRange"][0].as<double>();
  pars.zrange[1] = node["ZRange"][1].as<double>();

  pars.trange[0] = node["TRange"][0].as<double>();
  pars.trange[1] = node["TRange"][1].as<double>();
  
  pars.add_root = node["AddParent"].as<bool>(false);

  for (auto const &p : node["Particles"]) {
    pars.part_param_v.push_back(_parse_particle(p));
  }

  return pars;
}
