#include "EDepSimSDFactory.hh"
#include "EDepSimSegmentSD.hh"
#include "EDepSimSurfaceSD.hh"
#include "EDepSimException.hh"

#include <G4SDManager.hh>

EDepSim::SDFactory::SDFactory(G4String type)
    : fType(type) {}

EDepSim::SDFactory::~SDFactory() {}

G4VSensitiveDetector* EDepSim::SDFactory::GetSD(G4String name, G4String type) {
    G4VSensitiveDetector* sd
        = G4SDManager::GetSDMpointer()->FindSensitiveDetector(name);
    // Check that the type is correct
    if (type == "undefined") {
        type = fType;
    }
    if (type =="segment") {
        EDepSim::SegmentSD* seg = dynamic_cast<EDepSim::SegmentSD*>(sd);
        if (sd != nullptr && seg == nullptr) {
            EDepSimError("SD " << name << " is not a SegmentSD");
        }
    }
    else if (type == "surface") {
        EDepSim::SurfaceSD* sur = dynamic_cast<EDepSim::SurfaceSD*>(sd);
        if (sd != nullptr && sur == nullptr) {
            EDepSimError("SD " << name << " is not a SurfaceSD");
        }
    }
    else if (type != "undefined") {
        EDepSimError("SD " << type << " is an unrecognized type");
    }

    return sd;
}

G4VSensitiveDetector* EDepSim::SDFactory::MakeSD(G4String name, G4String type) {
    G4VSensitiveDetector* sd = GetSD(name,type);
    if (!sd) {
        if (type == "undefined") {
            type = fType;
        }
        if (type == "segment") {
            sd = new EDepSim::SegmentSD(name);
        }
        else if (type == "surface") {
            sd = new EDepSim::SurfaceSD(name);
        }
        else {
            EDepSimError("No such sensitive detector type:" << name);
            EDepSimThrow("sensitive detector type not implemented");
        }
        G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    }
    return sd;
}
