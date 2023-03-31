////////////////////////////////////////////////////////////
//

#include "EDepSimLog.hh"

#include "EDepSimRootPersistencyManager.hh"
#include "EDepSimRootGeometryManager.hh"
#include "EDepSimUserPrimaryGeneratorAction.hh"
#include "kinem/EDepSimKinemPassThrough.hh"
#include "kinem/EDepSimPrimaryGenerator.hh"
#include "kinem/EDepSimRooTrackerKinematicsGenerator.hh"
#include "kinem/EDepSimVKinematicsGenerator.hh"

#include <globals.hh>

#include <G4Event.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>


EDepSim::RootPersistencyManager::RootPersistencyManager() 
    : EDepSim::PersistencyManager(), fOutput(NULL), fEventTree(NULL) {}

EDepSim::RootPersistencyManager::~RootPersistencyManager() {
    if (fOutput) delete fOutput;
    fOutput = NULL;
}

bool EDepSim::RootPersistencyManager::IsOpen() {
    if (fOutput && fOutput->IsOpen()) {
        fOutput->cd();
        return true;
    }
    return false;
}

bool EDepSim::RootPersistencyManager::Open(G4String filename) {
    if (fOutput) {
        EDepSimLog("EDepSim::RootPersistencyManager::Open "
                 << "-- Delete current file pointer" );
    }

    SetFilename(filename);

    EDepSimLog("EDepSim::RootPersistencyManager::Open " << GetFilename());

    fOutput = TFile::Open(GetFilename(), "RECREATE", "EDepSim Root Output");
    fOutput->cd();
    
    fEventTree = new TTree("EDepSimEvents",
                           "Energy Deposition for Simulated Events");

    static TG4Event *pEvent = &fEventSummary;
    fEventTree->Branch("Event","TG4Event",&pEvent);
       
    fEventsNotSaved = 0;

    return true;
}

bool EDepSim::RootPersistencyManager::Close() {
    if (!fOutput) {
        EDepSimError("EDepSim::RootPersistencyManager::Close "
                   << "-- No Output File");
        return false;
    }

    fOutput->cd();

    fOutput->Write();
    fOutput->Close();

    fEventTree = NULL;

    return true;
}

bool EDepSim::RootPersistencyManager::Store(const G4Event* anEvent) {
    if (!fOutput) {
        EDepSimError("EDepSim::RootPersistencyManager::Store "
                   << "-- No Output File");
        return false;
    }

    bool save = UpdateSummaries(anEvent);

    if (save) {
        fOutput->cd();
        fEventTree->Fill();

        auto genAction = dynamic_cast<const EDepSim::UserPrimaryGeneratorAction*>(
            G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

        for (int i = 0; i < genAction->GetGeneratorCount(); ++i) {
            auto gen = dynamic_cast<const EDepSim::PrimaryGenerator*>(
                genAction->GetGenerator(i));
            auto vkinGen = const_cast<EDepSim::VKinematicsGenerator*>(
                gen->GetKinematicsGenerator());
            auto kinGen = dynamic_cast<EDepSim::RooTrackerKinematicsGenerator*>(
                vkinGen);

            if (kinGen) {
                EDepSim::KinemPassThrough::GetInstance()->AddEntry(
                    kinGen, fEventSummary.EventId);
                break;
            }
        }
    }

    return true;
}

bool EDepSim::RootPersistencyManager::Store(const G4Run*) {
    return false;
}

bool EDepSim::RootPersistencyManager::Store(const G4VPhysicalVolume*) {
    if (!fOutput) {
        EDepSimError("EDepSim::RootPersistencyManager::Store "
                   << "-- No Output File");
        return false;
    }
    if (!gGeoManager) {
        EDepSimError("EDepSim::RootPersistencyManage::Store(world)"
                  << " -- Cannot be run before /edep/update");
        return false; 
    }
    fOutput->cd();
    gGeoManager->Write();
    return true;
}

