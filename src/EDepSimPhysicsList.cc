#include "EDepSimPhysicsList.hh"
#include "EDepSimPhysicsListMessenger.hh"
#include "EDepSimException.hh"
#include "EDepSimExtraPhysics.hh"
#include "EDepSimDokeBirksSaturation.hh"

#include <EDepSimLog.hh>

#include <G4OpticalPhysics.hh>

#include <G4LossTableManager.hh>

#include <G4ProcessManager.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>

#include <G4PhysListFactory.hh>

#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>

#include <G4ProcessTable.hh>

#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>

#include <unistd.h>

EDepSim::PhysicsList::PhysicsList(G4String physName)
    : G4VModularPhysicsList() {
    G4LossTableManager::Instance();
    defaultCutValue  = 0.5*mm;
    fCutForGamma     = defaultCutValue;
    fCutForElectron  = defaultCutValue;
    fCutForPositron  = defaultCutValue;

    fMessenger = new EDepSim::PhysicsListMessenger(this);

    SetVerboseLevel(1);

    G4PhysListFactory factory;
    G4VModularPhysicsList* phys = NULL;

    // Check to see if the physics list has set the environment variable
    // PHYSLIST
    if (!phys) {
        char* list = getenv("PHYSLIST");
        if (list) {
            EDepSimLog("Set the physics list from the PHYSLIST environment: "
                       << list);
            phys = factory.ReferencePhysList();
        }
    }

    // Check if a list name was provided on the command line.  It usually is
    // not provided.
    if (!phys && physName.size() > 1) {
        EDepSimLog("Set the physics list from the command line: " << physName);
        if (!factory.IsReferencePhysList(physName)) {
            EDepSimError("Not a supported physics list");
            EDepSimThrow("Unsupported physics list");
        }
        phys =factory.GetReferencePhysList(physName);
    }

    // Use the default physics list.
    if (!phys) {
        physName = "QGSP_BERT";
        EDepSimLog("Use the default physics list: " << physName);
        phys =factory.GetReferencePhysList(physName);
    }

    if (!phys) {
        EDepSimThrow("No physics list was created.");
    }

    // Transfer the physics list from the factory to this one.
    for (G4int i = 0; ; ++i) {
        G4VPhysicsConstructor* elem =
            const_cast<G4VPhysicsConstructor*> (phys->GetPhysics(i));
        if (elem == NULL) break;
        G4cout << "RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
        RegisterPhysics(elem);
    }

    // Add the optical physics so photons can be created.
    RegisterPhysics(new G4OpticalPhysics());

    // Add our specific lists.
    fExtra = new EDepSim::ExtraPhysics();
    RegisterPhysics(fExtra);

    // Setup the parameters (override if necesssary)
    G4EmParameters* emParams = G4EmParameters::Instance();
    emParams->SetEmSaturation(new EDepSim::DokeBirksSaturation(0));

    // Force any necessary optical parameters.
    G4OpticalParameters* opParams = G4OpticalParameters::Instance();

    // Control with the macro file, not here!
    // opParams->SetBoundaryInvokeSD(true);

}

EDepSim::PhysicsList::~PhysicsList() {
    delete fMessenger;
}

void EDepSim::PhysicsList::SetCuts() {
    if (verboseLevel >0) {
        G4cout << "EDepSim::PhysicsList::SetCuts:";
        G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length")
               << G4endl;
    }

    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(fCutForGamma, "gamma");
    SetCutValue(fCutForElectron, "e-");
    SetCutValue(fCutForPositron, "e+");

    if (verboseLevel>0) DumpCutValuesTable();
}

void EDepSim::PhysicsList::SetCutForGamma(G4double cut) {
    fCutForGamma = cut;
    SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

void EDepSim::PhysicsList::SetCutForElectron(G4double cut) {
    fCutForElectron = cut;
    SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

void EDepSim::PhysicsList::SetCutForPositron(G4double cut) {
    fCutForPositron = cut;
    SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

void EDepSim::PhysicsList::SetIonizationModel(bool b) {
    fExtra->SetIonizationModel(b);
}
