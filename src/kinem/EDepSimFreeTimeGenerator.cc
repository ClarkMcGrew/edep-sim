#include "kinem/EDepSimVTimeGenerator.hh"
#include "kinem/EDepSimFreeTimeGenerator.hh"

EDepSim::FreeTimeGenerator::FreeTimeGenerator(G4String name) 
    : EDepSim::VTimeGenerator(name) {}

EDepSim::FreeTimeGenerator::~FreeTimeGenerator() {}

double EDepSim::FreeTimeGenerator::GetTime(const G4LorentzVector& /* vtx */) {
    return 0.0;
}

bool EDepSim::FreeTimeGenerator::ForceTime() {
    return false;
}
