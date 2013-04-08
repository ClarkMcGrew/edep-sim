#include "kinem/DSimFixedPositionGenerator.hh"

DSimFixedPositionGenerator::DSimFixedPositionGenerator(
    const G4String& name, const G4ThreeVector& pos) 
    : DSimVPositionGenerator(name), fPosition(pos) {}

DSimFixedPositionGenerator::~DSimFixedPositionGenerator() {}

G4LorentzVector DSimFixedPositionGenerator::GetPosition() {
    return fPosition;
}

bool DSimFixedPositionGenerator::ForcePosition() {
    return true;
}

void DSimFixedPositionGenerator::SetPosition(const G4ThreeVector& pos) {
    fPosition.set(pos.x(),pos.y(),pos.z(),0);
}
