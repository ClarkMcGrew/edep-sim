#include "EDepSimExtraPhysics.hh"
#include "EDepSimException.hh"

#ifndef EDEPSIM_SKIP_NESTVersion098
// Include NEST
#include "NESTVersion098/G4S1Light.hh"
#include "NESTVersion098/G4ThermalElectron.hh"
#endif

#include "EDepSimSecondaryEnergy.hh"
#include "EDepSimLog.hh"

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

EDepSim::ExtraPhysics::ExtraPhysics()
    : G4VPhysicsConstructor("EDepSimExtra"), fIonizationModel(1) { }

EDepSim::ExtraPhysics::~ExtraPhysics() { }

void EDepSim::ExtraPhysics::ConstructParticle() {
#ifndef EDEPSIM_SKIP_NESTVersion098
    G4ThermalElectron::Definition();
#endif
}

void EDepSim::ExtraPhysics::ConstructProcess() {
    EDepSimLog("EDepSim::ExtraPhysics:: Add Extra Physics Processes");

    G4ParticleTable::G4PTblDicIterator* theParticleIterator
        = theParticleTable->GetIterator();

    theParticleIterator->reset();
    while ((*theParticleIterator)()) {
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pman = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();
        G4String particleType = particle->GetParticleType();
        double charge = particle->GetPDGCharge();

        if (!pman) {
            EDepSimError("Particle "
                      << particleName
                      << " without a Process Manager.");
            EDepSimThrow("Particle without a Process Manager.");
        }

        // All charged particles should have a step limiter to make sure that
        // the steps do not get too long.
        if (std::abs(charge) > 0.1) {
            pman->AddDiscreteProcess(new G4StepLimiter("Step Limit"));
        }

        switch (fIonizationModel) {
        case 0: {
#ifndef EDEPSIM_SKIP_NESTVersion098
            // Add nest to any applicable particle.
            G4S1Light* scintProcess = new G4S1Light();
            scintProcess->SetScintillationYieldFactor(1.0);
            if (scintProcess->IsApplicable(*particle)) {
                pman->AddProcess(scintProcess,ordDefault,
                                 ordInActive,ordDefault);
            }
#else
            EDepSimThrow("NEST not enabled during compilation");
#endif
            break;
        }
        case 1: default: {
            // Add EDepSim::SecondaryEnergy to any applicable particle.
            EDepSim::SecondaryEnergy* secondaryProcess
                = new EDepSim::SecondaryEnergy();
            if (secondaryProcess->IsApplicable(*particle)) {
                pman->AddProcess(secondaryProcess,ordDefault,
                                 ordInActive,ordDefault);
            }
            break;
        }
        }
    }
}
