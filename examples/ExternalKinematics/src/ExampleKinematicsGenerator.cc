#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>

#include "ExampleKinematicsGenerator.hh"

ExampleKinematicsGenerator::ExampleKinematicsGenerator(const G4String& name)
    : EDepSim::VKinematicsGenerator(name) {}

ExampleKinematicsGenerator::~ExampleKinematicsGenerator() {}

EDepSim::VKinematicsGenerator::GeneratorStatus
ExampleKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* evt, const G4LorentzVector& suggestedPosition) {

    EDepSimLog("Generate an external primary vertex");

    G4PrimaryVertex* theNewVertex
        = new G4PrimaryVertex(suggestedPosition.x(),
                              suggestedPosition.y(),
                              suggestedPosition.z(),
                              suggestedPosition.z());

    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();

    G4ParticleDefinition *particleDef = particleTable->FindParticle("mu+");

    G4ThreeVector theMomentum(0.0*MeV, 0.0*MeV, 200.0*MeV);

    G4PrimaryParticle* theNewParticle
        = new G4PrimaryParticle(particleDef,
                                theMomentum.x(),
                                theMomentum.y(),
                                theMomentum.z());

    theNewVertex->SetPrimary(theNewParticle);

    evt->AddPrimaryVertex(theNewVertex);

    return kSuccess;
}

extern "C"
EDepSim::VKinematicsGenerator* CreateKinematicsGenerator(const char* name) {
    return new ExampleKinematicsGenerator(std::string(name));
}
