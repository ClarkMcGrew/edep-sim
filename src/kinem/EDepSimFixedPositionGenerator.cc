#include "kinem/EDepSimFixedPositionGenerator.hh"

EDepSim::FixedPositionGenerator::FixedPositionGenerator(
    const G4String& name, const G4ThreeVector& pos) 
    : EDepSim::VPositionGenerator(name), fPosition(pos) {}

EDepSim::FixedPositionGenerator::~FixedPositionGenerator() {}

G4LorentzVector EDepSim::FixedPositionGenerator::GetPosition() {
    return fPosition;
}

bool EDepSim::FixedPositionGenerator::ForcePosition() {
    return true;
}

void EDepSim::FixedPositionGenerator::SetPosition(const G4ThreeVector& pos) {
    fPosition.set(pos.x(),pos.y(),pos.z(),0);
}
