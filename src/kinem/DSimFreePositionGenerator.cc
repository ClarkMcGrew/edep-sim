#include "kinem/DSimFreePositionGenerator.hh"

DSimFreePositionGenerator::DSimFreePositionGenerator(const G4String& name) 
    : DSimVPositionGenerator(name) {}

DSimFreePositionGenerator::~DSimFreePositionGenerator() {}

G4LorentzVector DSimFreePositionGenerator::GetPosition() {
    return G4LorentzVector(0,0,0,0);
}

bool DSimFreePositionGenerator::ForcePosition() {
    return false;
}
