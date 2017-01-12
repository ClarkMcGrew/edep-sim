#include "CaptPMTBuilder.hh"
#include "EDepSimBuilder.hh"

#include "EDepSimLog.hh"

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <G4Polyhedra.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>

#include <cmath>

class CaptPMTMessenger
    : public EDepSim::BuilderMessenger {
private:
    CaptPMTBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fSizeCMD;
    G4UIcmdWithADoubleAndUnit* fBaseLengthCMD;
    G4UIcmdWithABool*          fRoundCMD;

public:
    CaptPMTMessenger(CaptPMTBuilder* c) 
        : EDepSim::BuilderMessenger(c,"Control the PMT construction."),
          fBuilder(c) {

        fSizeCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("size"),this);
        fSizeCMD->SetGuidance("Set the PMT size.");
        fSizeCMD->SetParameterName("size",false);
        fSizeCMD->SetUnitCategory("Length");

        fBaseLengthCMD = new G4UIcmdWithADoubleAndUnit(
            CommandName("baseLength"),this);
        fBaseLengthCMD->SetGuidance("Set the PMT base length.");
        fBaseLengthCMD->SetParameterName("length",false);
        fBaseLengthCMD->SetUnitCategory("Length");

        fRoundCMD = new G4UIcmdWithABool(
            CommandName("round"),this);
        fRoundCMD->SetGuidance("Flag that the PMT is round.");
    }

    virtual ~CaptPMTMessenger() {
        delete fSizeCMD;
        delete fBaseLengthCMD;
        delete fRoundCMD;
    }

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fSizeCMD) {
            fBuilder->SetSize(fSizeCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fBaseLengthCMD) {
            fBuilder->SetBaseLength(fBaseLengthCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fRoundCMD) {
            fBuilder->SetRound(fRoundCMD->GetNewBoolValue(val));
        }
        else {
            EDepSim::BuilderMessenger::SetNewValue(cmd,val);
        }
    }

};

void CaptPMTBuilder::Init(void) {
    SetMessenger(new CaptPMTMessenger(this));
    SetSize(25*CLHEP::mm);
    SetBaseLength(25*CLHEP::mm);
}

CaptPMTBuilder::~CaptPMTBuilder() {}

G4LogicalVolume *CaptPMTBuilder::GetPiece(void) {
    const double glassThickness = 3*CLHEP::mm;

    G4LogicalVolume* logVolume 
        = new G4LogicalVolume(new G4Tubs(GetName(),
                                         0.0, GetSize()/2.0, 
                                         GetBaseLength()/2, 
                                         0*CLHEP::degree, 360*CLHEP::degree),
                              FindMaterial("Glass"),
                              GetName());
    logVolume->SetVisAttributes(GetColor(logVolume));

    // Construct the photo cathode volume.
    std::string namePhotoCathode = GetName() + "/PhotoCathode";
    G4LogicalVolume* logPhotoCathode 
        = new G4LogicalVolume(new G4Tubs(namePhotoCathode,
                                         0.0, GetSize()/2.0, 
                                         glassThickness/2.0, 
                                         0*CLHEP::degree, 360*CLHEP::degree),
                              FindMaterial("Glass"),
                              namePhotoCathode);
    logPhotoCathode->SetVisAttributes(GetColor(logPhotoCathode));

    // Place the vessel components.
    new G4PVPlacement(NULL,                     // rotation.
                      G4ThreeVector(0,0,
                                    GetBaseLength()/2.0 - glassThickness/2.0),
                      logPhotoCathode,            // logical volume
                      logPhotoCathode->GetName(), // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.
    
    // Construct the vacuum.
    std::string namePMTVoid = GetName() + "/PMTVoid";
    G4LogicalVolume* logPMTVoid
        = new G4LogicalVolume(new G4Tubs(namePMTVoid,
                                         0.0, GetSize()/2.0-glassThickness, 
                                         GetBaseLength()/2.0-glassThickness, 
                                         0*CLHEP::degree, 360*CLHEP::degree),
                              FindMaterial("Air"),  // should be vacuum...
                              namePMTVoid);
    logPMTVoid->SetVisAttributes(GetColor(logPMTVoid));

    // Place the vessel components.
    new G4PVPlacement(NULL,                     // rotation.
                      G4ThreeVector(0,0,0),
                      logPMTVoid,            // logical volume
                      logPMTVoid->GetName(), // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.
    
    return logVolume;
}
