#include "DSimExtraPhysics.hh"
#include "DSimException.hh"

#include <DSimLog.hh>

#include <globals.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleWithCuts.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessVector.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>
#include <G4Material.hh>
#include <G4ios.hh>
#include <G4StepLimiter.hh>

DSimExtraPhysics::DSimExtraPhysics() 
    : G4VPhysicsConstructor("DSim Extra") { }

DSimExtraPhysics::~DSimExtraPhysics() { }

void DSimExtraPhysics::ConstructParticle() { }

void DSimExtraPhysics::ConstructProcess() {
    DSimLog("DSimExtraPhysics:: Add Extra Physics Processes");

    theParticleIterator->reset();
    while ((*theParticleIterator)()) {
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pman = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();
        G4String particleType = particle->GetParticleType();
        double charge = particle->GetPDGCharge();

        if (!pman) {
            DSimError("Particle " 
                      << particleName 
                      << " without a Process Manager.");
            DSimThrow("Particle without a Process Manager.");
        }

        // All charged particles should have a step limiter to make sure that
        // the steps do not get too long.
        if (std::abs(charge) > 0.1) {
            pman->AddDiscreteProcess(new G4StepLimiter("Step Limit"));
        }
    }
}
