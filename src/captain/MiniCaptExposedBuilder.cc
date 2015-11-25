#include "MiniCaptExposedBuilder.hh"
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

class MiniCaptExposedMessenger
    : public DSimBuilderMessenger {
private:
    MiniCaptExposedBuilder* fBuilder;

public:
    MiniCaptExposedMessenger(MiniCaptExposedBuilder* c) 
        : DSimBuilderMessenger(c,"Control the exposed geometry."),
          fBuilder(c) {};

    virtual ~MiniCaptExposedMessenger() {};

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        DSimBuilderMessenger::SetNewValue(cmd,val);
    };
};

void MiniCaptExposedBuilder::Init(void) {
    SetMessenger(new MiniCaptExposedMessenger(this));
}

MiniCaptExposedBuilder::~MiniCaptExposedBuilder() {};

double MiniCaptExposedBuilder::GetRadius() {
    return 65*cm;
}

double MiniCaptExposedBuilder::GetHeight() {
    return 3*cm;
}

G4LogicalVolume *MiniCaptExposedBuilder::GetPiece(void) {

    G4LogicalVolume* logVolume 
        = new G4LogicalVolume(new G4Tubs(GetName(),
                                         0.0, GetRadius(), GetHeight()/2, 
                                         0*degree, 360*degree),
                              FindMaterial("Argon_Gas"),
                              GetName());
    logVolume->SetVisAttributes(GetColor(logVolume));

    G4ThreeVector center(0.0,0.0,-GetHeight()/2);
    fOffset = center;
    
    /// All the space above the drift region.
    center += G4ThreeVector(0.0,0.0,0.0);

    return logVolume;
}
