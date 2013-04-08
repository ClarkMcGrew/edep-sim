#include "DSimBuilder.hh"
#include "DSimException.hh"
#include "DSimRootGeometryManager.hh"

#include <algorithm>
#include <functional>

DSimBuilder::DSimBuilder(G4String n, 
                         DSimUserDetectorConstruction* c) 
    : fLocalName(n), fName(n), fConstruction(c), fParent(NULL),
      fMessenger(NULL), fVisible(false) {
    fMessenger = fConstruction->GetMessenger();
}

DSimBuilder::DSimBuilder(G4String n, DSimBuilder* p) 
    : fLocalName(n), fName(n), fConstruction(NULL), fParent(p), 
      fMessenger(NULL), fVisible(false) {
    fName = fParent->GetName() + "/" + fLocalName;
    fConstruction = fParent->GetConstruction();
    fMessenger = fParent->GetMessenger();
}

DSimBuilder::~DSimBuilder() {;};

G4LogicalVolume *DSimBuilder::GetPiece(void) {
    DSimError("DSimBuilder::GetPiece(): Not implemented");
    return NULL;
}

G4Material* DSimBuilder::FindMaterial(G4String name) {
    G4Material* material = G4Material::GetMaterial(name,true);
    return material;
}

G4String DSimBuilder::GetName(void) {
    return fName;
}

/// Set the local name of the object to be built.  The local name of the
/// logical volume being set, and also sets the full name of the volume.
void DSimBuilder::SetLocalName(const G4String& name) {
    fLocalName = name;
    fName = name;
    if (fParent) fName = fParent->GetName() + "/" + fName;
}


/// Set the visibility of the constructed object.
void DSimBuilder::SetVisible(bool v) {
    if (fVisible != v) {
        CaptVerbose("Set Visibility for " << GetName()
                    << " from " << fVisible << " to " << v);
    }
    fVisible = v;
}

/// Set the visibility of the constructed object.
void DSimBuilder::SetVisibleDaughters(bool v) {
    if (fVisible != v) {
        CaptVerbose("Set daughter visibility for " << GetName()
                    << " to " << v);
    }

    SetVisible(false);

    for (std::map<G4String,DSimBuilder*>::iterator p 
             = fSubBuilders.begin();
         p!=fSubBuilders.end();
         ++p) {
        (*p).second->SetVisible(v);
    }
}

DSimBuilderMessenger::DSimBuilderMessenger(DSimBuilder* c,
                                           const char* guide) {
    fBuilder = c;
    fDirName = c->GetName() + "/";
    fDirectory = new G4UIdirectory(fDirName);
    if (guide) {
        fDirectory->SetGuidance(guide);
    }
    else {
        std::string guidance = "Commands for the " + c->GetName() + " Builder";
        fDirectory->SetGuidance(guidance.c_str());
    }
    
    fVisibleCMD = new G4UIcmdWithABool(CommandName("visible"),this);
    fVisibleCMD->SetGuidance("The object is drawn if this is true.");
    fVisibleCMD->SetParameterName("visibility",false);
    
    fVisibleDaughtersCMD = new G4UIcmdWithABool(
        CommandName("visibleDaughters"),this);
    fVisibleDaughtersCMD->SetGuidance(
        "The daughters of this object are drawn if this is true.");
    fVisibleDaughtersCMD->SetParameterName("visibility",false);

    fCheckCMD = new G4UIcmdWithABool(CommandName("check"),this);
    fCheckCMD->SetGuidance("If this is true then check for overlaps");
    fCheckCMD->SetParameterName("check",false);
}

void DSimBuilderMessenger::SetNewValue(G4UIcommand *cmd, G4String val) {
    if (cmd == fVisibleCMD) {
        fBuilder->SetVisible(fVisibleCMD->GetNewBoolValue(val));
    }
    else if (cmd == fVisibleDaughtersCMD) {
        fBuilder->SetVisibleDaughters(
            fVisibleDaughtersCMD->GetNewBoolValue(val));
    }
    else if (cmd == fCheckCMD) {
        fBuilder->SetCheck(fCheckCMD->GetNewBoolValue(val));
    }
}

DSimBuilderMessenger::~DSimBuilderMessenger() {
    delete fDirectory;
    delete fVisibleCMD;
    delete fVisibleDaughtersCMD;
    delete fCheckCMD;
}

G4VisAttributes DSimBuilder::GetColor(G4Material* mat) {
    DSimRootGeometryManager* geoMan = DSimRootGeometryManager::Get();
    return G4VisAttributes(geoMan->GetG4Color(mat));
}

G4VisAttributes DSimBuilder::GetColor(G4LogicalVolume* vol) {
    DSimRootGeometryManager* geoMan = DSimRootGeometryManager::Get();
    G4Material* mat = vol->GetMaterial();
    return G4VisAttributes(geoMan->GetG4Color(mat));
}
