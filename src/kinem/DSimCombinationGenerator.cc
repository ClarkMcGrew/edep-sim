#include <G4PrimaryVertex.hh>
#include <G4Event.hh>

#include "DSimLog.hh"
#include "DSimException.hh"
#include "DSimCombinationGenerator.hh"

DSimCombinationGenerator::DSimCombinationGenerator(int src, int dest,
                                                   bool relative)
    : fSource(src), fDestination(dest), fRelative(relative) { }

DSimCombinationGenerator::~DSimCombinationGenerator(void) {}

void DSimCombinationGenerator::GeneratePrimaryVertex(G4Event* evt) {

    if (fSource < 0 || fSource >= evt->GetNumberOfPrimaryVertex()) {
        abort();
    }
    
    if (fDestination < 0 || fDestination >= evt->GetNumberOfPrimaryVertex()) {
        abort();
    }
    
    G4ThreeVector src = evt->GetPrimaryVertex(fSource)->GetPosition();
    double srcTime = evt->GetPrimaryVertex(fSource)->GetT0();

    if (fRelative) {
        G4ThreeVector dest = evt->GetPrimaryVertex(fDestination)->GetPosition();
        double destTime = evt->GetPrimaryVertex(fDestination)->GetT0();
        src = src + dest;
        srcTime = srcTime + destTime;
    }

    evt->GetPrimaryVertex(fDestination)->SetPosition(src.x(),src.y(),src.z());
    evt->GetPrimaryVertex(fDestination)->SetT0(srcTime);

}

G4String DSimCombinationGenerator::GetName() {
    return "combination";
}
