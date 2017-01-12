#include "EDepSimBuilder.hh"
#include "EDepSimException.hh"
#include "EDepSimRootGeometryManager.hh"
#include "EDepSimSDFactory.hh"
#include "EDepSimSegmentSD.hh"

#include <sstream>
#include <algorithm>
#include <functional>

EDepSim::Builder::Builder(G4String n, 
                         EDepSim::UserDetectorConstruction* c) 
    : fLocalName(n), fName(n), fConstruction(c), fParent(NULL),
      fMessenger(NULL), fSensitiveDetector(NULL), 
      fOpacity(0.0), fCheck(false) {
    fMessenger = fConstruction->GetMessenger();
}

EDepSim::Builder::Builder(G4String n, EDepSim::Builder* p) 
    : fLocalName(n), fName(n), fConstruction(NULL), fParent(p),
      fMessenger(NULL), fSensitiveDetector(NULL), 
      fOpacity(0.0), fCheck(false) {
    fName = fParent->GetName() + "/" + fLocalName;
    fConstruction = fParent->GetConstruction();
    // Provide a reasonable default.  This will (hopefully) be overridden in
    // Init().
    fMessenger = fParent->GetMessenger();
}

EDepSim::Builder::~Builder() {;}

G4LogicalVolume *EDepSim::Builder::GetPiece(void) {
    EDepSimThrow("EDepSim::Builder::GetPiece(): Not implemented");
    return NULL;
}

G4Material* EDepSim::Builder::FindMaterial(G4String name) {
    G4Material* material = G4Material::GetMaterial(name,true);
    return material;
}

G4String EDepSim::Builder::GetName(void) {
    return fName;
}

/// Set the local name of the object to be built.  The local name of the
/// logical volume being set, and also sets the full name of the volume.
void EDepSim::Builder::SetLocalName(const G4String& name) {
    fLocalName = name;
    fName = name;
    if (fParent) fName = fParent->GetName() + "/" + fName;
}


/// Set the visibility of the constructed object.
void EDepSim::Builder::SetOpacity(double v) {
    if (fOpacity != v) {
        EDepSimLog("Set relative opacity for " << GetName()
                    << " from " << fOpacity << " to " << v);
    }
    fOpacity = v;
}

/// Set the visibility of the constructed object.
void EDepSim::Builder::SetDaughterOpacity(double v) {
    for (std::map<G4String,EDepSim::Builder*>::iterator p 
             = fSubBuilders.begin();
         p!=fSubBuilders.end();
         ++p) {
        (*p).second->SetOpacity(v);
        (*p).second->SetDaughterOpacity(v);
    }
}

EDepSim::BuilderMessenger::BuilderMessenger(EDepSim::Builder* c,
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

void EDepSim::BuilderMessenger::SetNewValue(G4UIcommand *cmd, G4String val) {
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

EDepSim::BuilderMessenger::~BuilderMessenger() {
    delete fDirectory;
    delete fOpacityCMD;
    delete fDaughterOpacityCMD;
    delete fCheckCMD;
    delete fSensitiveCMD;
    delete fMaximumHitSagittaCMD;
    delete fMaximumHitLengthCMD;
}

G4VisAttributes EDepSim::Builder::GetColor(G4Material* mat, double opacity) {
    EDepSim::RootGeometryManager* geoMan = EDepSim::RootGeometryManager::Get();
    // Check if the opacity has been over-ruled by the macro...
    if (fOpacity < -9.9) return G4VisAttributes::Invisible;
    opacity += fOpacity;
    if (opacity < -9.9) return G4VisAttributes::Invisible;
    G4Color color = geoMan->GetG4Color(mat);
    double red = color.GetRed();
    double green = color.GetGreen();
    double blue = color.GetBlue();
    double alpha = std::max(0.0,color.GetAlpha());
    if (opacity > 9.9) alpha = 1.0;
    else if (fOpacity > 9.9) alpha = 1.0;
    else alpha = std::min(1.0, alpha*std::exp(opacity));
    return G4VisAttributes(G4Color(red,green,blue,alpha));
}

G4VisAttributes EDepSim::Builder::GetColor(G4LogicalVolume* vol, double opacity) {
    G4Material* mat = vol->GetMaterial();
    return GetColor(mat,opacity);
}

/// Set the sensitive detector for this component by name.
void EDepSim::Builder::SetSensitiveDetector(G4String name, G4String type) {
    EDepSim::SDFactory factory(type);
    SetSensitiveDetector(factory.MakeSD(name));
}
    
void EDepSim::Builder::SetMaximumHitSagitta(double sagitta) {
    if (!fSensitiveDetector) {
        EDepSimError("Maximum hit sagitta must be set after the sensitive"
                  " detector is defined.");
        EDepSimThrow("Builder does not have sensitive detector defined");
        return;
    }
    EDepSim::SegmentSD *segSD = dynamic_cast<EDepSim::SegmentSD*>(fSensitiveDetector);
    if (!segSD) {
        EDepSimThrow("Sensitive detector not derived from EDepSim::SegmentSD");
        return;
    }
    segSD->SetMaximumHitSagitta(sagitta);
}

void EDepSim::Builder::SetMaximumHitLength(double length) {
    if (!fSensitiveDetector) {
        EDepSimError("Maximum hit length must be set after the sensitive"
                  " detector is defined.");
        EDepSimThrow("Builder does not have sensitive detector defined");
        return;
    }
    EDepSim::SegmentSD *segSD = dynamic_cast<EDepSim::SegmentSD*>(fSensitiveDetector);
    if (!segSD) {
        EDepSimThrow("Sensitive detector not derived from EDepSim::SegmentSD");
        return;
    }
    segSD->SetMaximumHitLength(length);
}


