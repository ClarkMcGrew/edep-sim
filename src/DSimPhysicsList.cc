#include "DSimPhysicsList.hh"
#include "DSimPhysicsListMessenger.hh"
#include "DSimException.hh"
#include "DSimExtraPhysics.hh"

#include <DSimLog.hh>

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

#include <G4SystemOfUnits.hh>

void DSimPhysicsList::SetPhysicsListName(G4String pName) {
    DSimLog("Set the physics list name to '" << pName << "'");
    fPhysicsListName = pName;
}
 
DSimPhysicsList::DSimPhysicsList(G4String physName) 
    : G4VModularPhysicsList(), fPhysicsListName(physName) {
    G4LossTableManager::Instance();
    defaultCutValue  = 1.*mm;
    fCutForGamma     = defaultCutValue;
    fCutForElectron  = defaultCutValue;
    fCutForPositron  = defaultCutValue;

    fMessenger = new DSimPhysicsListMessenger(this);

    SetVerboseLevel(1);

    G4PhysListFactory factory;
    G4VModularPhysicsList* phys = NULL;
    
    if (fPhysicsListName.size()<1) {
        DSimLog("Set the default physics list");
        // fPhysicsListName = "Shielding";
        fPhysicsListName = "QGSP_BERT";
    }

    if (factory.IsReferencePhysList(fPhysicsListName)) {
        phys =factory.GetReferencePhysList(fPhysicsListName);
    }

    // A last resort.  The physics list might defined via environment variable
    // PHYSLIST
    if (!phys) {
        phys = factory.ReferencePhysList();
    }

    if (!phys) {
        DSimThrow("No physics list was created.");
    }

    // Transfer the physics list from the factory to this one.
    for (G4int i = 0; ; ++i) {
        G4VPhysicsConstructor* elem =
            const_cast<G4VPhysicsConstructor*> (phys->GetPhysics(i));
        if (elem == NULL) break;
        G4cout << "RegisterPhysics: " << elem->GetPhysicsName() << G4endl;
        RegisterPhysics(elem);
    }

    // Add our specific lists.
    RegisterPhysics(new DSimExtraPhysics());
    RegisterPhysics(new G4OpticalPhysics());

}

DSimPhysicsList::~DSimPhysicsList() {
    delete fMessenger;
}

void DSimPhysicsList::SetCuts() {
    if (verboseLevel >0) {
        G4cout << "DSimPhysicsList::SetCuts:";
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

void DSimPhysicsList::SetCutForGamma(G4double cut) {
    fCutForGamma = cut;
    SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

void DSimPhysicsList::SetCutForElectron(G4double cut) {
    fCutForElectron = cut;
    SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

void DSimPhysicsList::SetCutForPositron(G4double cut) {
    fCutForPositron = cut;
    SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

