#include "DSimUserPrimaryGeneratorMessenger.hh"
#include "DSimUserPrimaryGeneratorAction.hh"
#include "DSimException.hh"

#include "kinem/DSimPrimaryGenerator.hh"
#include "kinem/DSimVKinematicsGenerator.hh"

#include <TCaptLog.hxx>

#include <G4Event.hh>
#include <G4StateManager.hh>
#include <G4RunManager.hh>
#include <G4VPrimaryGenerator.hh>
#include <Randomize.hh>

DSimUserPrimaryGeneratorAction::DSimUserPrimaryGeneratorAction() {
    fMessenger = new DSimUserPrimaryGeneratorMessenger(this);
    fAllowEmptyEvents = false;
    fAddFakeGeantino = true;
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
                CaptSevere("Run aborted.");
                G4RunManager::GetRunManager()->AbortRun();
                return;
            }
        }
        if (fAddFakeGeantino) {
            CaptWarn("Add a GEANTINO vertex.");
            // Put the vertex far downstream, and long after beam spill.
            G4PrimaryVertex* theVertex = 
                new G4PrimaryVertex(G4ThreeVector(0*cm, 0*cm, 20*m),
                                    50000.0*ns);
            anEvent->AddPrimaryVertex(theVertex);
            // Create the GEANTINO
            G4ParticleTable* particleTable 
                = G4ParticleTable::GetParticleTable();
            G4ParticleDefinition* def = particleTable->FindParticle("geantino");
            G4PrimaryParticle* part 
                = new G4PrimaryParticle(def,0.0, 0.0, 1*MeV);
            theVertex->SetPrimary(part);
            break;
        }
        if (anEvent->GetNumberOfPrimaryVertex()>0) break;
        if (fAllowEmptyEvents) continue;
        CaptError("DSimUserPrimaryGeneratorAction::GeneratePrimaries(): "
                   << "Event generated without any primary verticies.");
        CaptError("No primaries generated.");
    }
}

void DSimUserPrimaryGeneratorAction::ClearGenerators() {
    for (std::vector<G4VPrimaryGenerator*>::iterator g 
             = fPrimaryGenerators.begin();
         g != fPrimaryGenerators.end();
         ++g) {
        delete (*g);
    }
    fPrimaryGenerators.clear();
}
