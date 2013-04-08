#include "kinem/DSimVTimeGenerator.hh"
#include "kinem/DSimFixedTimeGenerator.hh"

DSimFixedTimeGenerator::DSimFixedTimeGenerator(G4String name, double time) 
    : DSimVTimeGenerator(name), fTime(time) {}

DSimFixedTimeGenerator::~DSimFixedTimeGenerator() {}

double DSimFixedTimeGenerator::GetTime(const G4LorentzVector& vtx) {
    return fTime;
}

bool DSimFixedTimeGenerator::ForceTime() {
    return true;
}
