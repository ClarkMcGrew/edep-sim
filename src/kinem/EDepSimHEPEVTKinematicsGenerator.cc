#include <G4Event.hh>
#include <G4HEPEvtInterface.hh>

#include "EDepSimException.hh"

#include "kinem/EDepSimHEPEVTKinematicsGenerator.hh"

EDepSim::HEPEVTKinematicsGenerator::HEPEVTKinematicsGenerator(
    const G4String& name, const G4String& fileName, int verbosity)
    : EDepSim::VKinematicsGenerator(name), fGenerator(NULL) {
    EDepSimLog("Open HEPEVT file " << fileName);
    fGenerator = new G4HEPEvtInterface(fileName,verbosity);
}

EDepSim::HEPEVTKinematicsGenerator::~HEPEVTKinematicsGenerator() {
    if (fGenerator) delete fGenerator;
}

EDepSim::VKinematicsGenerator::GeneratorStatus
EDepSim::HEPEVTKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* evt, const G4LorentzVector& /* position */) {
    if (!fGenerator) {
        EDepSimThrow("EDepSim::HEPEVTKinematicsGenerator::"
                     " File Not Open");
    }
    try {
        fGenerator->GeneratePrimaryVertex(evt);
    }
    catch (...) {
        EDepSimError("Kaboom");
        exit(1);
    }

    return kSuccess;
}
