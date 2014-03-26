#include <DSimLog.hh>

#include "kinem/DSimUniformPositionGenerator.hh"

DSimUniformPositionGenerator::DSimUniformPositionGenerator(const G4String& name) 
    : DSimVConstrainedPositionGenerator(name) {}

DSimUniformPositionGenerator::~DSimUniformPositionGenerator() {}

G4LorentzVector DSimUniformPositionGenerator::GetPosition() {
    for (int finiteLoop = 0; finiteLoop<100000; ++finiteLoop) {
        G4LorentzVector vtx = TrialPosition();
        if (ValidPosition(vtx)) return vtx;
    }
    DSimError("DSimUniformPositionGenerator::GetPosition:"
              << " No valid position found");
    throw;
}

bool DSimUniformPositionGenerator::ForcePosition() {
    return true;
}
