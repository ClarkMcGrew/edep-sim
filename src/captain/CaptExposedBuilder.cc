#include "CaptExposedBuilder.hh"
#include "CaptPMTAssemblyBuilder.hh"

#include "DSimBuilder.hh"

#include "DSimLog.hh"

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

class CaptExposedMessenger
    : public DSimBuilderMessenger {
private:
    CaptExposedBuilder* fBuilder;

public:
    CaptExposedMessenger(CaptExposedBuilder* c) 
        : DSimBuilderMessenger(c,"Control the exposed geometry."),
          fBuilder(c) {

    };

    virtual ~CaptExposedMessenger() {
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        DSimBuilderMessenger::SetNewValue(cmd,val);
    };
};

void CaptExposedBuilder::Init(void) {
    SetMessenger(new CaptExposedMessenger(this));

    AddBuilder(new CaptPMTAssemblyBuilder("PMTAssembly",this));
}

CaptExposedBuilder::~CaptExposedBuilder() {}

double CaptExposedBuilder::GetRadius() {
    CaptPMTAssemblyBuilder& pmts = Get<CaptPMTAssemblyBuilder>("PMTAssembly");
    double radius = pmts.GetRadius() + 2*CLHEP::cm;
    return radius;
}

double CaptExposedBuilder::GetHeight() {
    CaptPMTAssemblyBuilder& pmts = Get<CaptPMTAssemblyBuilder>("PMTAssembly");
    double height = pmts.GetHeight();
    return height;
}

G4LogicalVolume *CaptExposedBuilder::GetPiece(void) {

    G4LogicalVolume* logVolume 
        = new G4LogicalVolume(new G4Tubs(GetName(),
                                         0.0, GetRadius(), GetHeight()/2, 
                                         0*CLHEP::degree, 360*CLHEP::degree),
                              FindMaterial("Argon_Gas"),
                              GetName());
    logVolume->SetVisAttributes(GetColor(logVolume));

    G4ThreeVector center(0.0,0.0,-GetHeight()/2);
    fOffset = center;
    
    /// All the space above the drift region.
    center += G4ThreeVector(0.0,0.0,0.0);

    // Put in the PMT mounting
    CaptPMTAssemblyBuilder& pmts = Get<CaptPMTAssemblyBuilder>("PMTAssembly");
    center += G4ThreeVector(0.0,0.0,pmts.GetHeight()/2);
    G4LogicalVolume* logPMTS = pmts.GetPiece();

    G4RotationMatrix* rotation = new G4RotationMatrix(); 
    rotation->rotateY(180*CLHEP::degree);

    new G4PVPlacement(rotation,                    // rotation.
                      center,                  // position
                      logPMTS,            // logical volume
                      logPMTS->GetName(), // name
                      logVolume,               // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    return logVolume;
}
