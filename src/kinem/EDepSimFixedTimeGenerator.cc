#include "kinem/EDepSimVTimeGenerator.hh"
#include "kinem/EDepSimFixedTimeGenerator.hh"

EDepSim::FixedTimeGenerator::FixedTimeGenerator(G4String name, double time) 
    : EDepSim::VTimeGenerator(name), fTime(time) {}

EDepSim::FixedTimeGenerator::~FixedTimeGenerator() {}

double EDepSim::FixedTimeGenerator::GetTime(const G4LorentzVector& /* vtx */) {
    return fTime;
}

bool EDepSim::FixedTimeGenerator::ForceTime() {
    return true;
}
