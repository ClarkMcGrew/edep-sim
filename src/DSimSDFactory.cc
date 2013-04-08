#include "DSimSDFactory.hh"
#include "DSimSegmentSD.hh"
#include "DSimException.hh"

#include <G4SDManager.hh>

DSimSDFactory::DSimSDFactory(G4String type) 
    : fType(type) {}


DSimSDFactory::~DSimSDFactory() {}

G4VSensitiveDetector* DSimSDFactory::GetSD(G4String name) {
    return G4SDManager::GetSDMpointer()->FindSensitiveDetector(name);
}

G4VSensitiveDetector* DSimSDFactory::MakeSD(G4String name) {
    G4VSensitiveDetector* sd = GetSD(name);
    if (!sd) {
        if (fType == "segment") {
            sd = new DSimSegmentSD(name);
        }
        else {
            CaptError("No such sensitive detector " << name);
            DSimError("sensitive detector type not implemented");
        }
        G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    }
    return sd;
}
