#include "DSimBuilder.hh"
#include "DSimException.hh"
#include "DSimRootGeometryManager.hh"
#include "DSimSDFactory.hh"
#include "DSimSegmentSD.hh"

#include <sstream>
#include <algorithm>
#include <functional>

DSimBuilder::DSimBuilder(G4String n, 
                         DSimUserDetectorConstruction* c) 
    : fLocalName(n), fName(n), fConstruction(c), fParent(NULL),
      fMessenger(NULL), fSensitiveDetector(NULL), 
      fVisible(false), fCheck(false) {
    fMessenger = fConstruction->GetMessenger();
}

DSimBuilder::DSimBuilder(G4String n, DSimBuilder* p) 
    : fLocalName(n), fName(n), fConstruction(NULL), fParent(p),
      fMessenger(NULL), fSensitiveDetector(NULL), 
      fVisible(false), fCheck(false) {
    fName = fParent->GetName() + "/" + fLocalName;
    fConstruction = fParent->GetConstruction();
    // Provide a reasonable default.  This will (hopefully) be overridden in
    // Init().
    fMessenger = fParent->GetMessenger();
}

DSimBuilder::~DSimBuilder() {;};

G4LogicalVolume *DSimBuilder::GetPiece(void) {
    DSimThrow("DSimBuilder::GetPiece(): Not implemented");
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
        DSimVerbose("Set Visibility for " << GetName()
                    << " from " << fVisible << " to " << v);
    }
    fVisible = v;
}

/// Set the visibility of the constructed object.
void DSimBuilder::SetVisibleDaughters(bool v) {
    if (fVisible != v) {
        DSimVerbose("Set daughter visibility for " << GetName()
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

    fSensitiveCMD = new G4UIcommand(CommandName("sensitive"),this);
    fSensitiveCMD->SetGuidance("Set the name of the sensitive detector");
    G4UIparameter* nameParam = new G4UIparameter('s');
    nameParam->SetParameterName("Name");
    fSensitiveCMD->SetParameter(nameParam);
    G4UIparameter* typeParam = new G4UIparameter('s');
    typeParam->SetParameterName("Type");
    fSensitiveCMD->SetParameter(typeParam);
    
    fMaximumHitSagittaCMD 
        = new G4UIcmdWithADoubleAndUnit(CommandName("maxHitSagitta"),this);
    fMaximumHitSagittaCMD->SetGuidance("Set the maximum sagitta for a hit.");
    fMaximumHitSagittaCMD->SetParameterName("Sagitta",false);
    fMaximumHitSagittaCMD->SetUnitCategory("Length");
    
    fMaximumHitLengthCMD 
        = new G4UIcmdWithADoubleAndUnit(CommandName("maxHitLength"),this);
    fMaximumHitLengthCMD->SetGuidance("Set the maximum length for a hit.");
    fMaximumHitLengthCMD->SetParameterName("HitLength",false);
    fMaximumHitLengthCMD->SetUnitCategory("Length");
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
    else if (cmd==fSensitiveCMD) {
        std::istringstream buf(val.c_str());
        std::string name, type;
        buf >> name;
        buf >> type;
        fBuilder->SetSensitiveDetector(name,type);
    }
    else if (cmd==fMaximumHitSagittaCMD) {
        fBuilder->
            SetMaximumHitSagitta(
                fMaximumHitSagittaCMD->GetNewDoubleValue(val));
    }
    else if (cmd==fMaximumHitLengthCMD) {
        fBuilder->
            SetMaximumHitLength(
                fMaximumHitLengthCMD->GetNewDoubleValue(val));
    }
}

DSimBuilderMessenger::~DSimBuilderMessenger() {
    delete fDirectory;
    delete fVisibleCMD;
    delete fVisibleDaughtersCMD;
    delete fCheckCMD;
    delete fSensitiveCMD;
    delete fMaximumHitSagittaCMD;
    delete fMaximumHitLengthCMD;
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

/// Set the sensitive detector for this component by name.
void DSimBuilder::SetSensitiveDetector(G4String name, G4String type) {
    DSimSDFactory factory(type);
    SetSensitiveDetector(factory.MakeSD(name));
}
    
void DSimBuilder::SetMaximumHitSagitta(double sagitta) {
    if (!fSensitiveDetector) {
        DSimError("Builder does not have sensitive detector defined");
        return;
    }
    DSimSegmentSD *segSD = dynamic_cast<DSimSegmentSD*>(fSensitiveDetector);
    if (!segSD) {
        DSimError("Sensitive detector not derived from DSimSegmentSD");
        return;
    }
    segSD->SetMaximumHitSagitta(sagitta);
}

void DSimBuilder::SetMaximumHitLength(double length) {
    if (!fSensitiveDetector) {
        DSimError("Builder does not have sensitive detector defined");
        return;
    }
    DSimSegmentSD *segSD = dynamic_cast<DSimSegmentSD*>(fSensitiveDetector);
    if (!segSD) {
        DSimError("Sensitive detector not derived from DSimSegmentSD");
        return;
    }
    segSD->SetMaximumHitLength(length);
}


