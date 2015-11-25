#include "MiniCaptImmersedBuilder.hh"
#include "CaptDriftRegionBuilder.hh"
#include "MiniCaptPMTAssemblyBuilder.hh"

#include "DSimBuilder.hh"

#include <DSimLog.hh>

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>

class MiniCaptImmersedMessenger
    : public DSimBuilderMessenger {
private:
    MiniCaptImmersedBuilder* fBuilder;

public:
    MiniCaptImmersedMessenger(MiniCaptImmersedBuilder* c) 
        : DSimBuilderMessenger(c,"Control the immersed geometry."),
          fBuilder(c) {

    };

    virtual ~MiniCaptImmersedMessenger() {
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        DSimBuilderMessenger::SetNewValue(cmd,val);
    };
};

void MiniCaptImmersedBuilder::Init(void) {
    SetMessenger(new MiniCaptImmersedMessenger(this));
    SetSensitiveDetector("cryo","segment");

    AddBuilder(new CaptDriftRegionBuilder("Drift",this));
    AddBuilder(new MiniCaptPMTAssemblyBuilder("PMTAssembly",this));

    /// Set the drift region parameters for CAPTAIN.
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    drift.SetApothem(503*mm);
    drift.SetDriftLength(320*mm);
    drift.SetWirePlaneSpacing(3.18*mm);

}

MiniCaptImmersedBuilder::~MiniCaptImmersedBuilder() {};

double MiniCaptImmersedBuilder::GetRadius() {
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    double radius = drift.GetRadius() + 2*cm;
    return radius;
}

double MiniCaptImmersedBuilder::GetHeight() {
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    MiniCaptPMTAssemblyBuilder& pmts
        = Get<MiniCaptPMTAssemblyBuilder>("PMTAssembly");
    double height = drift.GetHeight() + pmts.GetHeight();
    return height;
}

G4LogicalVolume *MiniCaptImmersedBuilder::GetPiece(void) {

    G4LogicalVolume* logVolume 
        = new G4LogicalVolume(new G4Tubs(GetName(),
                                         0.0, GetRadius(), GetHeight()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("Argon_Liquid"),
                              GetName());
    logVolume->SetVisAttributes(GetColor(logVolume));
    if (GetSensitiveDetector()) {
        logVolume->SetSensitiveDetector(GetSensitiveDetector());
    }

    G4ThreeVector center(0.0,0.0,GetHeight()/2);

    /// All the space above the drift region.
    center -= G4ThreeVector(0.0,0.0,0.0);

    // Put in the drift region.
    CaptDriftRegionBuilder& drift = Get<CaptDriftRegionBuilder>("Drift");
    G4LogicalVolume* logDrift = drift.GetPiece();
    center -= G4ThreeVector(0.0,0.0,drift.GetHeight()/2);
    fOffset = center + drift.GetOffset();
    new G4PVPlacement(NULL,                    // rotation.
                      center,                  // position
                      logDrift,                // logical volume
                      logDrift->GetName(),     // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    
    // Put in the field cage.
    double fieldInner[] = {drift.GetApothem()+10*mm, drift.GetApothem()+10*mm};
    double fieldOuter[] = {drift.GetApothem()+13*mm, drift.GetApothem()+13*mm};
    double fieldPlane[] = {-drift.GetHeight()/2,
                           drift.GetHeight()/2-drift.GetGridPlaneOffset()};

    G4LogicalVolume *logFieldCage
	= new G4LogicalVolume(new G4Polyhedra(GetName()+"/FieldCage",
                                              90*degree, 450*degree,
                                              6, 2,
                                              fieldPlane,
                                              fieldInner, fieldOuter),
                              FindMaterial("FR4_Copper"),
                              GetName()+"/FieldCage");
    logVolume->SetVisAttributes(GetColor(logVolume));

    new G4PVPlacement(NULL,                    // rotation.
                      center,                  // position
                      logFieldCage,            // logical volume
                      logFieldCage->GetName(), // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.
    
    // Put in the PMT mounting
    MiniCaptPMTAssemblyBuilder& pmts
        = Get<MiniCaptPMTAssemblyBuilder>("PMTAssembly");
    center -= G4ThreeVector(0.0,0.0,drift.GetHeight()/2);
    center -= G4ThreeVector(0.0,0.0,pmts.GetHeight()/2);
    G4LogicalVolume* logPMTS = pmts.GetPiece();
    new G4PVPlacement(NULL,                    // rotation.
                      center,                  // position
                      logPMTS,            // logical volume
                      logPMTS->GetName(), // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    return logVolume;
}
