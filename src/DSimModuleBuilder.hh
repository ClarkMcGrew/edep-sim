#ifndef DSimModuleBuilder_hh_seen
#define DSimModuleBuilder_hh_seen

#include <sstream>

#include <G4ThreeVector.hh>

class G4LogicalVolume;

#include "DSimBuilder.hh"
#include "DSimComponentBuilder.hh"

/// Construct a module from components.

class DSimModuleBuilder : public DSimBuilder {

public:

    DSimModuleBuilder(G4String n, DSimUserDetectorConstruction* c)
        : DSimBuilder(n,c) {Init();};

    DSimModuleBuilder(G4String n, DSimBuilder* p)
        : DSimBuilder(n,p) {
        Init();};

    virtual ~DSimModuleBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Set the width of the module.  This is the X dimension of the module.
    void SetWidth(double w) {fWidth = w;}
    /// Get the width of the module.
    double GetWidth(void) {return fWidth;}

    /// Set the height of the module.  This is the Y dimension of the module.
    void SetHeight(double w) {fHeight = w;}
    /// Get the height of the module.
    double GetHeight(void) {return fHeight;}

    /// Get the length of the module.
    double GetLength(void) {return fLength;};

    /// Clear the list of components to be added to the FG tracker.
    void ClearComponentList(void);

    /// Add a new component name to the FG tracker.  Components are added from
    /// upstream (-Z) to downstream (+Z). 
    void AddComponent(G4String m);

    /// Set the number of repetitions for the last components added to the
    /// module.  This counts the total repetitions of the last component,
    /// inclduing that component.  So "1" repetition doesn't increase the
    /// number of components.  This also controls the number of previous
    /// components to be part of the repetion.
    void SetRepetitions(int r, int c);

    /// Translate some components inside the module in X/Y 
    /// according to module number and total components in a module. 
    /// This simulates misalignment in FGD layers.
    void SetModuleCompTrans(int m, int cPerM, int c,
                            double transX, double transY);

    /// Set the target length of the module bounding box along the
    /// beam axis (the Z axis).  The actual length must be shorter than this.
    /// Note that the beam goes down at at a few degrees, but the Z axis is
    /// horizontal.
    void SetTargetLength(double w) {fTargetLength = w;}
    /// Get the target length of the module bounding box.
    double GetTargetLength(void) {return fTargetLength;}

    /// Set flag to accept the TargetLength as the actual module length.
    void SetFixLength(bool f) {fFixLength = f;}
    /// Get flag to accept the TargetLength as the actual module length.
    bool GetFixLength(void) {return fFixLength;}

    /// Set x position at which components are positioned inside the module.
    void SetXPosition(double x) {xPosition = x;}
    /// Get x position at which components are positioned inside the module.
    double GetXPosition() {return xPosition;}

    /// Set y position at which components are positioned inside the module.
    void SetYPosition(double y) {yPosition = y;}
    /// Get y position at which components are positioned inside the module.
    double GetYPosition() {return yPosition;}

    /// Get the maximum positive x translation parameter among the modules.
    double GetXmaxTranslation() {return xmax;}
    /// Get the minimum negative x translation parameter among the modules.
    double GetXminTranslation() {return xmin;}
    /// Get the maximum positive y translation parameter among the modules.
    double GetYmaxTranslation() {return ymax;}
    /// Get the minimum negative y translation parameter among the modules.
    double GetYminTranslation() {return ymin;}

private:

    /// The width of the module.
    double fWidth;
    
    /// The height of the module.
    double fHeight;
    
    /// The length of the module.
    double fLength;

    /// The target length of module bounding box (parallel to the
    /// beam axis).  The actual length must be shorter than this.
    double fTargetLength;

    /// Logical flag to accept the target length as the module length.
    bool fFixLength;

    /// The x position at which components are positioned inside the module.
    double xPosition;

    /// The y position at which components are positioned inside the module.
    double yPosition;

    /// Default pair of translation constants.
    std::pair<double, double> fPair;

    /// A vector of ComponentBuilders that will be added to the module.
    typedef std::vector<DSimComponentBuilder*> PartsList;
    PartsList* fPartsList;

    /// A vector of translation parameters in x and y for each component
    /// in the module. Used in parallel with fPartsList (same number of
    /// entries as fPartsList)
    typedef std::vector<std::pair<double, double> > TransList;
    TransList* fTransList;

    /// Collect the minimum and maximum of translations in x and y
    G4double xmax, xmin, ymax, ymin;

    void Init(void);
};

class DSimModuleBuilderMessenger: public DSimBuilderMessenger {
private:
    DSimModuleBuilder* fBuilder;
    G4UIcmdWithoutParameter*   fClearCMD;
    G4UIcmdWithAString*        fAddCMD;
    G4UIcommand*               fRepeatCMD;
    G4UIcmdWithADoubleAndUnit* fWidthCMD;
    G4UIcmdWithADoubleAndUnit* fHeightCMD;

public:
    DSimModuleBuilderMessenger(DSimModuleBuilder* c,
                                    const char* guide=NULL) 
        : DSimBuilderMessenger(c,guide),
          fBuilder(c) {
        fClearCMD = new G4UIcmdWithoutParameter(CommandName("clear"),this);
        fClearCMD->SetGuidance("Clear the component list for the FG tracker.");

        fAddCMD = new G4UIcmdWithAString(CommandName("add"),this);
        fAddCMD->SetGuidance("Add the named component to the module."
                             "  Components are added from upstream"
                             " to downstream.");
        fAddCMD->SetParameterName("Component", false);

        fRepeatCMD = new G4UIcommand(CommandName("repeat"),this);
        fRepeatCMD->SetGuidance("Times to repeat the last components.");
        G4UIparameter* repParam = new G4UIparameter('i');
        repParam->SetParameterName("Repetitions");
        fRepeatCMD->SetParameter(repParam);
        G4UIparameter* cntParam = new G4UIparameter('i');
        cntParam->SetParameterName("Count");
        fRepeatCMD->SetParameter(cntParam);

        fWidthCMD = new G4UIcmdWithADoubleAndUnit(CommandName("width"),this);
        fWidthCMD->SetGuidance("Set the width of the module.");
        fWidthCMD->SetParameterName("Width",false);
        fWidthCMD->SetUnitCategory("Length");

        fHeightCMD = new G4UIcmdWithADoubleAndUnit(CommandName("height"),this);
        fHeightCMD->SetGuidance("Set the height of the module.");
        fHeightCMD->SetParameterName("Height",false);
        fHeightCMD->SetUnitCategory("Length");
    };

    virtual ~DSimModuleBuilderMessenger() {
        delete fClearCMD;
        delete fAddCMD;
        delete fRepeatCMD;
        delete fWidthCMD;
        delete fHeightCMD;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fClearCMD) {
            fBuilder->ClearComponentList();
        }
        else if (cmd==fAddCMD) {
            fBuilder->AddComponent(val);
        }
        else if (cmd==fRepeatCMD) {
            int repetitions;
            int count = 1;
            std::istringstream inputs((char*)val.c_str());
            inputs >> repetitions >> count;
            fBuilder->SetRepetitions(repetitions,count);
        }
        else if (cmd==fWidthCMD) {
            fBuilder->SetWidth(fWidthCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fHeightCMD) {
            fBuilder->SetHeight(fHeightCMD->GetNewDoubleValue(val));
        }
        else {
            DSimBuilderMessenger::SetNewValue(cmd,val);
        }
    };
};

#endif
