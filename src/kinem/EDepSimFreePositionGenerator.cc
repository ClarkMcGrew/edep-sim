#include "kinem/EDepSimFreePositionGenerator.hh"

EDepSim::FreePositionGenerator::FreePositionGenerator(const G4String& name) 
    : EDepSim::VPositionGenerator(name) {}

EDepSim::FreePositionGenerator::~FreePositionGenerator() {}

G4LorentzVector EDepSim::FreePositionGenerator::GetPosition() {
    return G4LorentzVector(0,0,0,0);
}

bool EDepSim::FreePositionGenerator::ForcePosition() {
    return false;
}
