#include <EDepSimLog.hh>

#include "kinem/EDepSimUniformPositionGenerator.hh"

EDepSim::UniformPositionGenerator::UniformPositionGenerator(const G4String& name) 
    : EDepSim::VConstrainedPositionGenerator(name) {}

EDepSim::UniformPositionGenerator::~UniformPositionGenerator() {}

G4LorentzVector EDepSim::UniformPositionGenerator::GetPosition() {
    for (int finiteLoop = 0; finiteLoop<100000; ++finiteLoop) {
        G4LorentzVector vtx = TrialPosition();
        if (ValidPosition(vtx)) return vtx;
    }
    EDepSimError("EDepSim::UniformPositionGenerator::GetPosition:"
              << " No valid position found");
    throw;
}

bool EDepSim::UniformPositionGenerator::ForcePosition() {
    return true;
}
