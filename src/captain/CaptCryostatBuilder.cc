#include "CaptCryostatBuilder.hh"
#include "CaptDriftRegionBuilder.hh"

#include "DSimBuilder.hh"

#include <DSimLog.hh>

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>

class CaptCryostatMessenger
    : public DSimBuilderMessenger {
private:
    CaptCryostatBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fInnerDiameterCMD;
    G4UIcmdWithADoubleAndUnit* fInnerHeightCMD;
    G4UIcmdWithADoubleAndUnit* fWallThicknessCMD;
    G4UIcmdWithADoubleAndUnit* fArgonDepthCMD;

public:
    CaptCryostatMessenger(CaptCryostatBuilder* c) 
        : DSimBuilderMessenger(c,"Control the driftRegion geometry."),
          fBuilder(c) {

        fInnerDiameterCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("innerDiameter"),this);
        fInnerDiameterCMD->SetGuidance(
            "Set the inner diameter of the cryostat.  This is the\n"
            "diameter of the cold volume.");
        fInnerDiameterCMD->SetParameterName("diameter",false);
        fInnerDiameterCMD->SetUnitCategory("Length");

        fInnerHeightCMD 
            = new G4UIcmdWithADoubleAndUnit(CommandName("innerHeight"),this);
        fInnerHeightCMD->SetGuidance(
            "Set the inner height of the cryostat.  This is the height\n"
            "of the cold volume.");
        fInnerHeightCMD->SetParameterName("height",false);
        fInnerHeightCMD->SetUnitCategory("Length");

        fWallThicknessCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("wallThickness"),this);
        fWallThicknessCMD->SetGuidance(
            "Set the thickness of the SS wall of the cryostat.");
        fWallThicknessCMD->SetParameterName("thickness",false);
        fWallThicknessCMD->SetUnitCategory("Length");

        fArgonDepthCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("argonDepth"),this);
        fArgonDepthCMD->SetGuidance(
            "Set the depth of the liquid argon.");
        fArgonDepthCMD->SetParameterName("depth",false);
        fArgonDepthCMD->SetUnitCategory("Length");
 
    };

    virtual ~CaptCryostatMessenger() {
        delete fInnerDiameterCMD;
        delete fInnerHeightCMD;
        delete fWallThicknessCMD;
        delete fArgonDepthCMD;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fInnerDiameterCMD) {
            fBuilder->SetInnerDiameter(
                fInnerDiameterCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fInnerHeightCMD) {
            fBuilder->SetInnerHeight(
                fInnerHeightCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fWallThicknessCMD) {
            fBuilder->SetWallThickness(
                fWallThicknessCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fArgonDepthCMD) {
            fBuilder->SetArgonDepth(
                fArgonDepthCMD->GetNewDoubleValue(val));
        }
        else {
            DSimBuilderMessenger::SetNewValue(cmd,val);
        }
    };
};

void CaptCryostatBuilder::Init(void) {
    SetMessenger(new CaptCryostatMessenger(this));

    SetInnerDiameter(2800*mm);
    SetInnerHeight(2000*mm);
    SetWallThickness(15*mm);
    SetArgonDepth(1500*mm);

    SetSensitiveDetector("cryo","segment");

    AddBuilder(new CaptDriftRegionBuilder("Drift",this));

}

CaptCryostatBuilder::~CaptCryostatBuilder() {};

double CaptCryostatBuilder::GetDiameter() {
    // Calculate the total diameter of the cryostat based on it's other
    // dimensions.
    return GetInnerDiameter() + 2*GetWallThickness();
}

double CaptCryostatBuilder::GetHeight() {
    // Calculate the total height of the cryostat based on it's other
    // dimensions.
    return GetInnerHeight() + 2*GetWallThickness();
}

G4LogicalVolume *CaptCryostatBuilder::GetPiece(void) {

    G4LogicalVolume* logVolume 
        = new G4LogicalVolume(new G4Tubs(GetName(),
                                         0.0, GetDiameter()/2.0, GetHeight()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("Air"),
                              GetName());
    
    // Construct the vessel.  The vessel name is not used for anything, so you
    // have (almost) complete freedom in how it's constructed.  The only
    // constraint is that it needs to be hollow so that it can contain a
    // volume of gas, and the liquid argon.
    std::string nameVessel = GetName()+"/VesselSide";
    G4LogicalVolume* logVesselSide
        = new G4LogicalVolume(new G4Tubs(nameVessel,
                                         GetInnerDiameter()/2,
                                         (GetWallThickness() 
                                          + GetInnerDiameter()/2),
                                         GetInnerHeight()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("SS_304"),
                              nameVessel);

    nameVessel = GetName()+"/VesselTop";
    G4LogicalVolume* logVesselTop
        = new G4LogicalVolume(new G4Tubs(nameVessel,
                                         0, 
                                         (GetWallThickness() 
                                          + GetInnerDiameter()/2),
                                         GetWallThickness()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("SS_304"),
                              nameVessel);

    nameVessel = GetName()+"/VesselBottom";
    G4LogicalVolume* logVesselBottom
        = new G4LogicalVolume(new G4Tubs(nameVessel,
                                         0, 
                                         (GetWallThickness() 
                                          + GetInnerDiameter()/2),
                                         GetWallThickness()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("SS_304"),
                              nameVessel);

    // Place the vessel components.
    new G4PVPlacement(NULL,                     // rotation.
                      G4ThreeVector(0,0,0),     // position
                      logVesselSide,            // logical volume
                      logVesselSide->GetName(), // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.

    new G4PVPlacement(NULL,                    // rotation.
                      G4ThreeVector(0,0,       // position
                                    GetInnerHeight()/2
                                    +GetWallThickness()/2),    
                      logVesselTop,            // logical volume
                      logVesselTop->GetName(), // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    new G4PVPlacement(NULL,                       // rotation.
                      G4ThreeVector(0,0,       // position
                                    - GetInnerHeight()/2
                                    - GetWallThickness()/2),    
                      logVesselBottom,            // logical volume
                      logVesselBottom->GetName(), // name
                      logVolume,                  // mother  volume
                      false,                      // (not used)
                      0,                          // Copy number (zero)
                      Check());                   // Check overlaps.

    // Make the argon gas volume at the top.  The important part is that this
    // needs to match the inside shape of the vessel.
    double gasThickness = GetInnerHeight() - GetArgonDepth();
    G4LogicalVolume* logAr
        = new G4LogicalVolume(new G4Tubs(GetName()+"/Gas",
                                         0.0,
                                         GetInnerDiameter()/2,
                                         gasThickness/2, 
                                         0*degree, 360*degree),
                              FindMaterial("Argon_Gas"),
                              GetName()+"/Gas");
    
    new G4PVPlacement(NULL,                    // rotation.
                      G4ThreeVector(0,0,       // position
                                    GetInnerHeight()/2
                                    -gasThickness/2),    
                      logAr,                   // logical volume
                      logAr->GetName(),        // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    // Make the argon liquid volume at the bottom.  The important part is that
    // this needs to match the inside shape of the vessel.
    G4LogicalVolume* logLAr
        = new G4LogicalVolume(new G4Tubs(GetName()+"/Liquid",
                                         0.0, 
                                         GetInnerDiameter()/2,
                                         GetArgonDepth()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("Argon_Liquid"),
                              GetName()+"/Liquid");

    if (GetSensitiveDetector()) {
        logLAr->SetSensitiveDetector(GetSensitiveDetector());
    }

    new G4PVPlacement(NULL,                    // rotation.
                      G4ThreeVector(0,0,       // position
                                    -GetInnerHeight()/2
                                    + GetArgonDepth()/2),    
                      logLAr,                   // logical volume
                      logLAr->GetName(),        // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    G4LogicalVolume* logDrift = drift.GetPiece();

    new G4PVPlacement(NULL,                    // rotation.
                      G4ThreeVector(0,0,       // position
                                    - GetArgonDepth()/2
                                    + drift.GetHeight()/2
                                    + 100*mm),    
                      logDrift,                // logical volume
                      logDrift->GetName(),     // name
                      logLAr,                  // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.


    logVolume->SetVisAttributes(G4VisAttributes::Invisible);
    logAr->SetVisAttributes(G4VisAttributes::Invisible);
    if (GetVisible()) {
        logVesselSide->SetVisAttributes(GetColor(logVesselSide));
        logVesselTop->SetVisAttributes(GetColor(logVesselSide));
        logVesselBottom->SetVisAttributes(GetColor(logVesselBottom));
        logLAr->SetVisAttributes(GetColor(logLAr));
    } 
    else {
        logVesselSide->SetVisAttributes(G4VisAttributes::Invisible);
        logVesselTop->SetVisAttributes(G4VisAttributes::Invisible);
        logVesselBottom->SetVisAttributes(G4VisAttributes::Invisible);
        logLAr->SetVisAttributes(G4VisAttributes::Invisible);
    }
    
    return logVolume;
}
