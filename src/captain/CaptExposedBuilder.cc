#include "CaptExposedBuilder.hh"

#include "EDepSimBuilder.hh"

#include "EDepSimLog.hh"

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
    : public EDepSim::BuilderMessenger {
private:
    CaptExposedBuilder* fBuilder;

public:
    CaptExposedMessenger(CaptExposedBuilder* c)
        : EDepSim::BuilderMessenger(c,"Control the exposed geometry."),
          fBuilder(c) {

    };

    virtual ~CaptExposedMessenger() {
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        EDepSim::BuilderMessenger::SetNewValue(cmd,val);
    };
};

void CaptExposedBuilder::Init(void) {
    SetMessenger(new CaptExposedMessenger(this));

}

CaptExposedBuilder::~CaptExposedBuilder() {}

double CaptExposedBuilder::GetRadius() {
    double radius = 120*CLHEP::cm;
    return radius;
}

double CaptExposedBuilder::GetHeight() {
    return 20*CLHEP::cm;
}

G4LogicalVolume *CaptExposedBuilder::GetPiece(void) {

    std::cout << " Radius " << GetRadius() << std::endl;
    std::cout << " Height " << GetHeight() << std::endl;
    G4LogicalVolume* logVolume
        = new G4LogicalVolume(new G4Tubs(GetName(),
                                         0.0, GetRadius(), GetHeight()/2,
                                         0*CLHEP::degree, 360*CLHEP::degree),
                              FindMaterial("Argon_Gas"),
                              GetName());
    logVolume->SetVisAttributes(GetColor(logVolume));

    G4ThreeVector center(0.0,0.0,-GetHeight()/2-0.1*CLHEP::mm);
    fOffset = center;

    /// All the space above the drift region.
    center += G4ThreeVector(0.0,0.0,0.0);

    return logVolume;
}
