////////////////////////////////////////////////////////////
//

#include "DSimLog.hh"

#include "DSimRootPersistencyManager.hh"
#include "DSimRootGeometryManager.hh"

#include <globals.hh>

#include <G4Event.hh>
#include <G4Run.hh>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>


DSimRootPersistencyManager::DSimRootPersistencyManager() 
    : DSimPersistencyManager(), fOutput(NULL), fEventTree(NULL) {}

DSimRootPersistencyManager::~DSimRootPersistencyManager() {
    if (fOutput) delete fOutput;
    fOutput = NULL;
}

bool DSimRootPersistencyManager::IsOpen() {
    if (fOutput && fOutput->IsOpen()) {
        fOutput->cd();
        return true;
    }
    return false;
}

bool DSimRootPersistencyManager::Open(G4String filename) {
    if (fOutput) {
        DSimLog("DSimRootPersistencyManager::Open "
                 << "-- Delete current file pointer" );
    }

    SetFilename(filename +".root");

    DSimLog("DSimRootPersistencyManager::Open " << GetFilename());

    fOutput = TFile::Open(GetFilename(), "RECREATE", "DSim Root Output");

    fEventTree = new TTree("dsimEventTree","Tree of Events");

    static TG4Event *pEventSummary = &fEventSummary;
    fEventTree->Branch("EventSummary","TG4Event",&pEventSummary);

    fEventsNotSaved = 0;

    return true;
}

bool DSimRootPersistencyManager::Close() {
    if (!fOutput) {
        DSimError("DSimRootPersistencyManager::Close "
                   << "-- No Output File");
        return false;
    }

    fOutput->cd();
    fOutput->Write();
    fOutput->Close();
    DSimLog("Output file " << GetFilename() << " closed.");

    fEventTree = NULL;

    return true;
}

bool DSimRootPersistencyManager::Store(const G4Event* anEvent) {
    if (!fOutput) {
        DSimError("DSimRootPersistencyManager::Store "
                   << "-- No Output File");
        return false;
    }

    UpdateSummaries(anEvent);
    
    fOutput->cd();

    fEventTree->Fill();

    return true;
}

bool DSimRootPersistencyManager::Store(const G4Run*) {
    return false;
}

bool DSimRootPersistencyManager::Store(const G4VPhysicalVolume* aWorld) {
    if (!fOutput) {
        DSimError("DSimRootPersistencyManager::Store "
                   << "-- No Output File");
        return false;
    }
    if (!gGeoManager) {
        DSimError("DSimRootPersistencyManage::Store(world)"
                  << " -- Cannot be run before /dsim/update");
        return false; 
    }
    fOutput->cd();
    gGeoManager->Write();
    return true;
}

