#include "MiniCaptExposedBuilder.hh"

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

class MiniCaptExposedMessenger
    : public EDepSim::BuilderMessenger {
private:
    MiniCaptExposedBuilder* fBuilder;

public:
    MiniCaptExposedMessenger(MiniCaptExposedBuilder* c) 
        : EDepSim::BuilderMessenger(c,"Control the exposed geometry."),
          fBuilder(c) {};

    virtual ~MiniCaptExposedMessenger() {};

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        EDepSim::BuilderMessenger::SetNewValue(cmd,val);
    };
};

void MiniCaptExposedBuilder::Init(void) {
    SetMessenger(new MiniCaptExposedMessenger(this));
}

MiniCaptExposedBuilder::~MiniCaptExposedBuilder() {}

double MiniCaptExposedBuilder::GetRadius() {
    return 65*CLHEP::cm;
}

double MiniCaptExposedBuilder::GetHeight() {
    return 3*CLHEP::cm;
}

G4LogicalVolume *MiniCaptExposedBuilder::GetPiece(void) {

#ifdef MAKE_EMPTY_EXPOSED
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
#else
    G4LogicalVolume* logVolume = NULL;
#endif
    
    return logVolume;
}
