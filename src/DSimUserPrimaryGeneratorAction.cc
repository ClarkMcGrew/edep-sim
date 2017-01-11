#include "DSimUserPrimaryGeneratorMessenger.hh"
#include "DSimUserPrimaryGeneratorAction.hh"
#include "DSimException.hh"

#include "kinem/DSimPrimaryGenerator.hh"
#include "kinem/DSimVKinematicsGenerator.hh"

#include <DSimLog.hh>

#include <G4Event.hh>
#include <G4StateManager.hh>
#include <G4RunManager.hh>
#include <G4VPrimaryGenerator.hh>
#include <Randomize.hh>

DSimUserPrimaryGeneratorAction::DSimUserPrimaryGeneratorAction() {
    fMessenger = new DSimUserPrimaryGeneratorMessenger(this);
    fAllowEmptyEvents = true;
    fAddFakeGeantino = false;
}

DSimUserPrimaryGeneratorAction::~DSimUserPrimaryGeneratorAction() {
    delete fMessenger;
}

void DSimUserPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    // Make sure that at least one generater is in the list.  If the list is
    // empty, then create the default generator.
    if (fPrimaryGenerators.size()<1) {
        AddGenerator(fMessenger->CreateGenerator());
    }

    for (int finiteLoop = 0; finiteLoop<1000; ++finiteLoop) {
        for (std::vector<G4VPrimaryGenerator*>::iterator generator
                 = fPrimaryGenerators.begin();
             generator != fPrimaryGenerators.end(); 
             ++generator) {
            try {
                (*generator)->GeneratePrimaryVertex(anEvent);
            }
            catch (DSimNoMoreEvents) {
                DSimLog("Run aborted.  No more input events.");
                G4RunManager::GetRunManager()->AbortRun();
                return;
            }
        }
        if (fAddFakeGeantino) {
            DSimWarn("Add a GEANTINO vertex.");
            // Put the vertex far way, and far in the future. 
            G4PrimaryVertex* theVertex = 
                new G4PrimaryVertex(G4ThreeVector(1E+20, 1E+20, 1E+20),
                                    1E+20);
            anEvent->AddPrimaryVertex(theVertex);
            // Create the GEANTINO
            G4ParticleTable* particleTable 
                = G4ParticleTable::GetParticleTable();
            G4ParticleDefinition* def = particleTable->FindParticle("geantino");
            G4PrimaryParticle* part 
                = new G4PrimaryParticle(def,0.0, 0.0, 1.0);
            theVertex->SetPrimary(part);
            break;
        }
        if (anEvent->GetNumberOfPrimaryVertex()>0) break;
        if (fAllowEmptyEvents) continue;
        DSimError("DSimUserPrimaryGeneratorAction::GeneratePrimaries(): "
                   << "Event generated without any primary verticies.");
        DSimThrow("No primaries generated.");
    }
}

void DSimUserPrimaryGeneratorAction::ClearGenerators() {
    for (std::vector<G4VPrimaryGenerator*>::iterator gen 
             = fPrimaryGenerators.begin();
         gen != fPrimaryGenerators.end();
         ++gen) {
        delete (*gen);
    }
    fPrimaryGenerators.clear();
}
