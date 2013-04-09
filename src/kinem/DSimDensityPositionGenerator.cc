#include <G4TransportationManager.hh>
#include <G4Navigator.hh>
#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisExtent.hh>
#include <Randomize.hh>

#include "DSimLog.hh"

#include "kinem/DSimDensityPositionGenerator.hh"

DSimDensityPositionGenerator::DSimDensityPositionGenerator(const G4String& name) 
    : DSimVConstrainedPositionGenerator(name), fMaximumDensity(-1) {}

DSimDensityPositionGenerator::~DSimDensityPositionGenerator() {}

G4LorentzVector DSimDensityPositionGenerator::GetPosition() {
    if (fMaximumDensity < 0) {
        const G4MaterialTable* mTable = G4Material::GetMaterialTable();
        for (G4MaterialTable::const_iterator m = mTable->begin();
             m != mTable->end();
             ++m) {
            fMaximumDensity = std::max(fMaximumDensity, (*m)->GetDensity());
        }
    }
    // Get the navigator.
    G4Navigator* navigator 
        = G4TransportationManager::GetTransportationManager()
        ->GetNavigatorForTracking();
    for (int finiteLoop = 0; finiteLoop<100000; ++finiteLoop) {
        G4LorentzVector vtx = TrialPosition();

        // Get the volume that contains the point.
        G4VPhysicalVolume* volume = navigator->LocateGlobalPointAndSetup(vtx);

        // Skip this vertex if it's not in a legal volume.
        if (!volume) continue;
            
        // Get the density.
        double density
            = volume->GetLogicalVolume()->GetMaterial()->GetDensity();

        // Skip this vertex if it misses the sampling.
        if (density < fMaximumDensity*G4UniformRand()) continue;

        // The vertex meets the density sampling, so check if it is valid.
        if (ValidPosition(vtx)) return vtx;
    }
    DSimWarn("DSimDensityPositionGenerator::GetPosition:"
              << " No valid position found");
    throw;
}

bool DSimDensityPositionGenerator::ForcePosition() {
    return true;
}
