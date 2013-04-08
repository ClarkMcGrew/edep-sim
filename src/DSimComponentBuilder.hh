#ifndef DSimComponentBuilder_hh_seen
#define DSimComponentBuilder_hh_seen
// $Id: DSimComponentBuilder.hh,v 1.7 2009/02/12 17:16:40 mcgrew Exp $
//

#include <G4ThreeVector.hh>
class G4LogicalVolume;

#include "DSimBuilder.hh"
#include "DSimSDFactory.hh"
#include "DSimSegmentSD.hh"

class G4VSensitiveDetector;

/// Construct a component to be used by a DSimModuleBuilder to build a
/// component.
class DSimComponentBuilder : public DSimBuilder {
public:
    DSimComponentBuilder(G4String n, DSimUserDetectorConstruction* c)
        : DSimBuilder(n,c), fLength(0.0), 
          fMaximumWidth(0.0), fMaximumHeight(0.0),
          fSensitiveDetector(NULL), 
          fMaximumHitLength(5*mm), fMaximumHitSagitta(1*mm) {}
    DSimComponentBuilder(G4String n, DSimBuilder* p)
        : DSimBuilder(n,p), fLength(0.0),
          fMaximumWidth(0.0), fMaximumHeight(0.0),
          fSensitiveDetector(NULL), 
          fMaximumHitLength(5*mm), fMaximumHitSagitta(1*mm) {}
    virtual ~DSimComponentBuilder() {;};

    /// Set the width of the component.  This is the X dimension of the space
    /// available for the component and may be relative to the outside geometry.
    /// The width is set by the DSimModuleBuilder object which will
    /// contain this component, so it should not be directly set in user code.
    /// The actual constructed component width must be less than the width set
    /// by the owning DSimModuleBuilder.
    virtual void SetWidth(double w) {fWidth = w;}

    /// Get the width of the component.  This is the X dimension of the space
    /// available for the component and may be relative to the outside geometry.
    /// The width is set by the DSimModuleBuilder object which will
    /// contain this component.  The actual constructed component width must
    /// be less than the width set by the owning DSimModuleBuilder.
    virtual double GetWidth(void) {return fWidth;}

    /// Set the physical width of the component, this may be smaller than the
    /// available width set in SetWidth.  This is the X dimension of the
    /// component.
    virtual void SetMaximumWidth(double w) {fMaximumWidth = w;}

    /// Set the height of the component.  This is the Y dimension of the space
    /// available for the component and may be relative to the outside
    /// geometry.  The height is set by the DSimModuleBuilder object
    /// which will contain this component, so it should not be directly set in
    /// user code.  The actual constructed component height must be less than
    /// the height set by the owning DSimModuleBuilder.
    virtual void SetHeight(double w) {fHeight = w;}

    /// Get the height of the component.  This is the Y dimension of the space
    /// available for the component and may be relative to the outside
    /// geometry.  The actual constructed component height must be less than
    /// the height set by the owning DSimModuleBuilder.
    virtual double GetHeight(void) {return fHeight;}

    /// Get the length of the component.
    virtual double GetLength(void) {return fLength;}

    /// Set the length of the component.
    virtual void SetLength(double t) {fLength = t;}

    /// Set the physical height of the component, this must be less than or
    /// equal to the available height set in SetHeight (by the parent
    /// DSimModuleBuilder object). This is the Y dimension of the
    /// component.
    virtual void SetMaximumHeight(double h) {fMaximumHeight = h;}

    /// This is the width of the component that will be actually constructed.
    /// This can be used in GetPiece() to find the size of the bounding box.
    virtual double GetPhysicalWidth() {
        double w = fWidth;
        if (fMaximumWidth>0 && fMaximumWidth<w) w = fMaximumWidth;
        return w;
    }

    /// This is the height of the component that will be actually constructed.
    /// This can be used in GetPiece() to find the size of the bounding box.
    virtual double GetPhysicalHeight() {
        double h = fHeight;
        if (fMaximumHeight>0 && fMaximumHeight<h) h = fMaximumHeight;
        return h;
    }

    /// Set the sensitive detector for this component.
    virtual void SetSensitiveDetector(G4VSensitiveDetector* s) {
        fSensitiveDetector = s;
    }

    /// Set the sensitive detector for this component by name.
    virtual void SetSensitiveDetector(G4String name, G4String type) {
        DSimSDFactory factory(type);
        SetSensitiveDetector(factory.MakeSD(name));
        SetMaximumHitSagitta(fMaximumHitSagitta);
        SetMaximumHitLength(fMaximumHitLength);
    }

    /// Get the sensitive detector for this component.
    virtual G4VSensitiveDetector* GetSensitiveDetector(void) {
        return fSensitiveDetector;
    }

    virtual void SetMaximumHitSagitta(double sagitta) {
        fMaximumHitSagitta = sagitta;
        DSimSegmentSD *segSD 
            = dynamic_cast<DSimSegmentSD*>(fSensitiveDetector);
        if (segSD) {
            segSD->SetMaximumHitSagitta(fMaximumHitSagitta);
        }
    }

    virtual void SetMaximumHitLength(double length) {
        fMaximumHitLength = length;
        DSimSegmentSD *segSD 
            = dynamic_cast<DSimSegmentSD*>(fSensitiveDetector);
        if (segSD) {
            segSD->SetMaximumHitLength(fMaximumHitLength);
        }
    }

protected:
    /// Increment the length of the component.  This is the distance the
    /// component stretchs along the Z axis of the detector.
    virtual void AddLength(double t) {fLength += t;};

private:
    /// The available width of the component in the X direction.
    double fWidth;

    /// The available height of the component in the Y direction.
    double fHeight;

    /// The length of the component along the Z axis.  
    double fLength;

    /// The maximum physical width of the constructed component.  This must be
    /// less than the available width.
    double fMaximumWidth;

    /// The maximum physical height of the constructed component.  This must be
    /// less than the available height.
    double fMaximumHeight;

    /// The sensitive detector for this tracking component.
    G4VSensitiveDetector* fSensitiveDetector;

    /// Control the total length of track that can be added to a single hit.
    double fMaximumHitLength;

    /// Control the maximum sagitta of a single hit.
    double fMaximumHitSagitta;

};

class DSimComponentBuilderMessenger: public DSimBuilderMessenger {
private:
    DSimComponentBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fWidthCMD;
    G4UIcmdWithADoubleAndUnit* fHeightCMD;
    G4UIcmdWithADoubleAndUnit* fMaxWidthCMD;
    G4UIcmdWithADoubleAndUnit* fMaxHeightCMD;
    G4UIcommand*               fSensitiveCMD;
    G4UIcmdWithADoubleAndUnit* fMaximumHitSagittaCMD;
    G4UIcmdWithADoubleAndUnit* fMaximumHitLengthCMD;

public:
    DSimComponentBuilderMessenger(DSimComponentBuilder* c,
                                       const char* guide) 
        : DSimBuilderMessenger(c,guide),
          fBuilder(c) {

        fWidthCMD = new G4UIcmdWithADoubleAndUnit(CommandName("width"),this);
        fWidthCMD->SetGuidance("Set the width of the component.");
        fWidthCMD->SetParameterName("Width",false);
        fWidthCMD->SetUnitCategory("Length");

        fHeightCMD = new G4UIcmdWithADoubleAndUnit(CommandName("height"),this);
        fHeightCMD->SetGuidance("Set the height of the component.");
        fHeightCMD->SetParameterName("Height",false);
        fHeightCMD->SetUnitCategory("Length");

        fMaxWidthCMD 
            = new G4UIcmdWithADoubleAndUnit(CommandName("maxWidth"),this);
        fMaxWidthCMD->SetGuidance("Set the maximum width of the component.");
        fMaxWidthCMD->SetParameterName("Width",false);
        fMaxWidthCMD->SetUnitCategory("Length");

        fMaxHeightCMD 
            = new G4UIcmdWithADoubleAndUnit(CommandName("maxHeight"),this);
        fMaxHeightCMD->SetGuidance("Set the maximum height of the component.");
        fMaxHeightCMD->SetParameterName("Height",false);
        fMaxHeightCMD->SetUnitCategory("Length");

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

    };

    virtual ~DSimComponentBuilderMessenger() {
        delete fWidthCMD;
        delete fHeightCMD;
        delete fMaxWidthCMD;
        delete fMaxHeightCMD;
        delete fSensitiveCMD;
        delete fMaximumHitSagittaCMD;
        delete fMaximumHitLengthCMD;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fWidthCMD) {
            fBuilder->SetWidth(fWidthCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fHeightCMD) {
            fBuilder->SetHeight(fHeightCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fMaxWidthCMD) {
            fBuilder->
                SetMaximumWidth(fMaxWidthCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fMaxHeightCMD) {
            fBuilder->
                SetMaximumHeight(fMaxHeightCMD->GetNewDoubleValue(val));
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
        else {
            DSimBuilderMessenger::SetNewValue(cmd,val);
        }
    };
};
#endif
