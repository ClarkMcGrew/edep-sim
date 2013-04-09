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
          fMaximumWidth(0.0), fMaximumHeight(0.0) {}
    DSimComponentBuilder(G4String n, DSimBuilder* p)
        : DSimBuilder(n,p), fLength(0.0),
          fMaximumWidth(0.0), fMaximumHeight(0.0) {}
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
};

class DSimComponentBuilderMessenger: public DSimBuilderMessenger {
private:
    DSimComponentBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fWidthCMD;
    G4UIcmdWithADoubleAndUnit* fHeightCMD;
    G4UIcmdWithADoubleAndUnit* fMaxWidthCMD;
    G4UIcmdWithADoubleAndUnit* fMaxHeightCMD;

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

    };

    virtual ~DSimComponentBuilderMessenger() {
        delete fWidthCMD;
        delete fHeightCMD;
        delete fMaxWidthCMD;
        delete fMaxHeightCMD;
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
        else {
            DSimBuilderMessenger::SetNewValue(cmd,val);
        }
    };
};
#endif
