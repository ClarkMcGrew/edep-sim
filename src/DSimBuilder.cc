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
      fOpacity(0.0), fCheck(false) {
    fMessenger = fConstruction->GetMessenger();
}

DSimBuilder::DSimBuilder(G4String n, DSimBuilder* p) 
    : fLocalName(n), fName(n), fConstruction(NULL), fParent(p),
      fMessenger(NULL), fSensitiveDetector(NULL), 
      fOpacity(0.0), fCheck(false) {
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
void DSimBuilder::SetOpacity(double v) {
    if (fOpacity != v) {
        DSimLog("Set relative opacity for " << GetName()
                    << " from " << fOpacity << " to " << v);
    }
    fOpacity = v;
}

/// Set the visibility of the constructed object.
void DSimBuilder::SetDaughterOpacity(double v) {
    for (std::map<G4String,DSimBuilder*>::iterator p 
             = fSubBuilders.begin();
         p!=fSubBuilders.end();
         ++p) {
        (*p).second->SetOpacity(v);
        (*p).second->SetDaughterOpacity(v);
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
    
    fOpacityCMD = new G4UIcmdWithADouble(CommandName("opacity"),this);
    fOpacityCMD->SetGuidance("Set the log of the relative opacity."
                             " Useful values are between [-0.5, 0.5], and"
                             " +/-10 makes the object opague or transparent."
                             " A value of 0 leaves the opacity unchanged.");
    fOpacityCMD->SetParameterName("opacity",false);
    
    fDaughterOpacityCMD = new G4UIcmdWithADouble(
        CommandName("daughterOpacity"),this);
    fDaughterOpacityCMD->SetGuidance(
        "Set the log of the relative opacity of the daughters.");
    fDaughterOpacityCMD->SetParameterName("opacity",false);

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
    if (cmd == fOpacityCMD) {
        fBuilder->SetOpacity(fOpacityCMD->GetNewDoubleValue(val));
    }
    else if (cmd == fDaughterOpacityCMD) {
        fBuilder->SetDaughterOpacity(
            fDaughterOpacityCMD->GetNewDoubleValue(val));
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
    delete fOpacityCMD;
    delete fDaughterOpacityCMD;
    delete fCheckCMD;
    delete fSensitiveCMD;
    delete fMaximumHitSagittaCMD;
    delete fMaximumHitLengthCMD;
}

G4VisAttributes DSimBuilder::GetColor(G4Material* mat, double opacity) {
    DSimRootGeometryManager* geoMan = DSimRootGeometryManager::Get();
    // Check if the opacity has been over-ruled by the macro...
    if (fOpacity < -9.9) return G4VisAttributes::Invisible;
    opacity += fOpacity;
    if (opacity < -9.9) return G4VisAttributes::Invisible;
    G4Color color = geoMan->GetG4Color(mat);
    double r = color.GetRed();
    double g = color.GetGreen();
    double b = color.GetBlue();
    double a = std::max(0.0,color.GetAlpha());
    if (opacity > 9.9) a = 1.0;
    else if (fOpacity > 9.9) a = 1.0;
    else a = std::min(1.0, a*std::exp(opacity));
    return G4VisAttributes(G4Color(r,g,b,a));
}

G4VisAttributes DSimBuilder::GetColor(G4LogicalVolume* vol, double opacity) {
    DSimRootGeometryManager* geoMan = DSimRootGeometryManager::Get();
    G4Material* mat = vol->GetMaterial();
    return GetColor(mat,opacity);
}

/// Set the sensitive detector for this component by name.
void DSimBuilder::SetSensitiveDetector(G4String name, G4String type) {
    DSimSDFactory factory(type);
    SetSensitiveDetector(factory.MakeSD(name));
}
    
void DSimBuilder::SetMaximumHitSagitta(double sagitta) {
    if (!fSensitiveDetector) {
        DSimError("Maximum hit sagitta must be set after the sensitive"
                  " detector is defined.");
        DSimThrow("Builder does not have sensitive detector defined");
        return;
    }
    DSimSegmentSD *segSD = dynamic_cast<DSimSegmentSD*>(fSensitiveDetector);
    if (!segSD) {
        DSimThrow("Sensitive detector not derived from DSimSegmentSD");
        return;
    }
    segSD->SetMaximumHitSagitta(sagitta);
}

void DSimBuilder::SetMaximumHitLength(double length) {
    if (!fSensitiveDetector) {
        DSimError("Maximum hit length must be set after the sensitive"
                  " detector is defined.");
        DSimThrow("Builder does not have sensitive detector defined");
        return;
    }
    DSimSegmentSD *segSD = dynamic_cast<DSimSegmentSD*>(fSensitiveDetector);
    if (!segSD) {
        DSimThrow("Sensitive detector not derived from DSimSegmentSD");
        return;
    }
    segSD->SetMaximumHitLength(length);
}


