#include "CaptDriftRegionBuilder.hh"
#include "CaptWirePlaneBuilder.hh"
#include "DSimBuilder.hh"

#include <DSimLog.hh>

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>

#include <G4Polyhedra.hh>

class CaptDriftRegionMessenger
    : public DSimBuilderMessenger {
private:
    CaptDriftRegionBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fApothemCMD;
    G4UIcmdWithADoubleAndUnit* fDriftLengthCMD;
    G4UIcmdWithADoubleAndUnit* fWirePlaneSpacingCMD;

public:
    CaptDriftRegionMessenger(CaptDriftRegionBuilder* c) 
        : DSimBuilderMessenger(c,"Control the drift region geometry."),
          fBuilder(c) {

        fApothemCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("apothem"),this);
        fApothemCMD->SetGuidance("Set the apothem of the drift region.");
        fApothemCMD->SetParameterName("apothem",false);
        fApothemCMD->SetUnitCategory("Length");

        fDriftLengthCMD = new G4UIcmdWithADoubleAndUnit(
            CommandName("driftLength"),this);
        fDriftLengthCMD->SetGuidance(
            "Set the drift length from cathode to wires.");
        fDriftLengthCMD->SetParameterName("drift",false);
        fDriftLengthCMD->SetUnitCategory("Length");

        fWirePlaneSpacingCMD = new G4UIcmdWithADoubleAndUnit(
            CommandName("wirePlaneSpacing"),this);
        fWirePlaneSpacingCMD->SetGuidance(
            "Set spacing between the wires.");
        fWirePlaneSpacingCMD->SetParameterName("space",false);
        fWirePlaneSpacingCMD->SetUnitCategory("Length");
    };

    virtual ~CaptDriftRegionMessenger() {
        delete fApothemCMD;
        delete fDriftLengthCMD;
        delete fWirePlaneSpacingCMD;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fApothemCMD) {
            fBuilder->SetApothem(fApothemCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fDriftLengthCMD) {
            fBuilder->SetDriftLength(fDriftLengthCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fWirePlaneSpacingCMD) {
            fBuilder->SetWirePlaneSpacing(
                fWirePlaneSpacingCMD->GetNewDoubleValue(val));
        }
        else {
            DSimBuilderMessenger::SetNewValue(cmd,val);
        }
    };
};

void CaptDriftRegionBuilder::Init(void) {
    SetMessenger(new CaptDriftRegionMessenger(this));
    SetApothem(1037.5*mm);
    SetDriftLength(1000*mm);
    SetWirePlaneSpacing(3.18*mm);
    SetSensitiveDetector("drift","segment");
    SetMaximumHitLength(1.0*mm);
    SetMaximumHitSagitta(0.25*mm);

    AddBuilder(new CaptWirePlaneBuilder("XPlane",this));
    AddBuilder(new CaptWirePlaneBuilder("VPlane",this));
    AddBuilder(new CaptWirePlaneBuilder("UPlane",this));
    AddBuilder(new CaptWirePlaneBuilder("GridPlane",this));
    AddBuilder(new CaptWirePlaneBuilder("GroundPlane",this));
}

CaptDriftRegionBuilder::~CaptDriftRegionBuilder() {};

double CaptDriftRegionBuilder::GetHeight() {
    CaptWirePlaneBuilder& wires = Get<CaptWirePlaneBuilder>("XPlane");
    return GetDriftLength() + GetGridPlaneOffset() + wires.GetHeight();
}

G4ThreeVector CaptDriftRegionBuilder::GetOffset() {
    double zOffset = GetHeight()/2.0;
    zOffset -= GetGridPlaneOffset();
    return G4ThreeVector(0.0, 0.0, zOffset);
}

double CaptDriftRegionBuilder::GetGroundPlaneOffset() {
    CaptWirePlaneBuilder& wires = Get<CaptWirePlaneBuilder>("XPlane");
    return 0.0*GetWirePlaneSpacing() + wires.GetHeight()/2;
}

double CaptDriftRegionBuilder::GetXPlaneOffset() {
    CaptWirePlaneBuilder& wires = Get<CaptWirePlaneBuilder>("XPlane");
    return 1.0*GetWirePlaneSpacing() + wires.GetHeight()/2;
}

double CaptDriftRegionBuilder::GetVPlaneOffset() {
    CaptWirePlaneBuilder& wires = Get<CaptWirePlaneBuilder>("XPlane");
    return 2.0*GetWirePlaneSpacing() + wires.GetHeight()/2;
}

double CaptDriftRegionBuilder::GetUPlaneOffset() {
    CaptWirePlaneBuilder& wires = Get<CaptWirePlaneBuilder>("XPlane");
    return 3.0*GetWirePlaneSpacing() + wires.GetHeight()/2;
}

double CaptDriftRegionBuilder::GetGridPlaneOffset() {
    CaptWirePlaneBuilder& wires = Get<CaptWirePlaneBuilder>("XPlane");
    return 4.0*GetWirePlaneSpacing() + wires.GetHeight()/2;
}

G4LogicalVolume *CaptDriftRegionBuilder::GetPiece(void) {

    double rInner[] = {0.0, 0.0};
    double rOuter[] = {GetApothem(), GetApothem()};
    double zPlane[] = {-GetHeight()/2, GetHeight()/2};

    DSimLog("Construct " << GetName() 
            << " with " << GetHeight()/mm << " mm height"
            << " and " <<  GetDriftLength()/mm << " mm drift");

    G4LogicalVolume *logVolume
	= new G4LogicalVolume(new G4Polyhedra(GetName(),
                                              90*degree, 450*degree,
                                              6, 2,
                                              zPlane, rInner, rOuter),
                              FindMaterial("Argon_Liquid"),
                              GetName());

    if (GetSensitiveDetector()) {
        logVolume->SetSensitiveDetector(GetSensitiveDetector());
        logVolume->SetUserLimits(new G4UserLimits(1.0*mm));
    }

    // The wire planes are rotated so that the local Z axis points into the
    // drift volume, and the local X axis points along the original X axis
    // (this puts the local Y axis along the -Y global axis.
    G4RotationMatrix* xRotation = new G4RotationMatrix(); 
    xRotation->rotateX(180*degree);

    CaptWirePlaneBuilder& xPlane = Get<CaptWirePlaneBuilder>("XPlane");
    xPlane.SetApothem(GetApothem());
    G4LogicalVolume *logX = xPlane.GetPiece();
    new G4PVPlacement(xRotation,                // rotation.
                      G4ThreeVector(0,0,
                                    (GetHeight()/2 - GetXPlaneOffset())), 
                      logX,                     // logical volume
                      logX->GetName(),          // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.
    

    G4RotationMatrix* vRotation = new G4RotationMatrix(); 
    vRotation->rotateX(180*degree);
    vRotation->rotateZ(-60*degree);
                       
    CaptWirePlaneBuilder& vPlane = Get<CaptWirePlaneBuilder>("VPlane");
    vPlane.SetApothem(GetApothem());
    G4LogicalVolume *logV = vPlane.GetPiece();
    new G4PVPlacement(vRotation,                // rotation.
                      G4ThreeVector(0,0,
                                    (GetHeight()/2 - GetVPlaneOffset())),
                      logV,                     // logical volume
                      logV->GetName(),          // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.


    G4RotationMatrix* uRotation = new G4RotationMatrix(); 
    uRotation->rotateX(180*degree);
    uRotation->rotateZ(60*degree);
    
    CaptWirePlaneBuilder& uPlane = Get<CaptWirePlaneBuilder>("UPlane");
    uPlane.SetApothem(GetApothem());
    G4LogicalVolume *logU = uPlane.GetPiece();
    new G4PVPlacement(uRotation,                // rotation.
                      G4ThreeVector(0,0,
                                    (GetHeight()/2 - GetUPlaneOffset())),
                      logU,                     // logical volume
                      logU->GetName(),          // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.
    
    CaptWirePlaneBuilder& gridPlane = Get<CaptWirePlaneBuilder>("GridPlane");
    gridPlane.SetApothem(GetApothem());
    G4LogicalVolume *logGrid = gridPlane.GetPiece();
    new G4PVPlacement(xRotation,                // rotation.
                      G4ThreeVector(0,0,
                                    (GetHeight()/2 - GetGridPlaneOffset())),
                      logGrid,                     // logical volume
                      logGrid->GetName(),          // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.
    
    CaptWirePlaneBuilder& grndPlane = Get<CaptWirePlaneBuilder>("GroundPlane");
    grndPlane.SetApothem(GetApothem());
    G4LogicalVolume *logGrnd = grndPlane.GetPiece();
    new G4PVPlacement(xRotation,                // rotation.
                      G4ThreeVector(0,0,
                                    (GetHeight()/2 - GetGroundPlaneOffset())),
                      logGrnd,                     // logical volume
                      logGrnd->GetName(),          // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.
    
    logVolume->SetVisAttributes(GetColor(logVolume));
    
    return logVolume;
}
