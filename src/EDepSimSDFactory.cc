#include "EDepSimSDFactory.hh"
#include "EDepSimSegmentSD.hh"
#include "EDepSimException.hh"

#include <G4SDManager.hh>

EDepSim::SDFactory::SDFactory(G4String type) 
    : fType(type) {}


EDepSim::SDFactory::~SDFactory() {}

G4VSensitiveDetector* EDepSim::SDFactory::GetSD(G4String name) {
    return G4SDManager::GetSDMpointer()->FindSensitiveDetector(name);
}

G4VSensitiveDetector* EDepSim::SDFactory::MakeSD(G4String name) {
    G4VSensitiveDetector* sd = GetSD(name);
    if (!sd) {
        if (fType == "segment") {
            sd = new EDepSim::SegmentSD(name);
        }
        else {
            EDepSimError("No such sensitive detector " << name);
            EDepSimThrow("sensitive detector type not implemented");
        }
        G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    }
    return sd;
}
