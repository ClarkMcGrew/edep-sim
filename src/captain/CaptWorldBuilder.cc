#include "DSimBuilder.hh"
#include "CaptWorldBuilder.hh"
#include "CaptCryostatBuilder.hh"

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4VisExtent.hh>

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>

#include <DSimLog.hh>

class CaptWorldMessenger
    : public DSimBuilderMessenger {
private:
    CaptWorldBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fLengthCMD;
    G4UIcmdWithADoubleAndUnit* fWidthCMD;
    G4UIcmdWithADoubleAndUnit* fHeightCMD;

public:
    CaptWorldMessenger(CaptWorldBuilder* c) 
        : DSimBuilderMessenger(c,"Control the world geometry."),
          fBuilder(c) {
        fLengthCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("length"),this);
        fLengthCMD->SetGuidance("Set the length of the world.");
        fLengthCMD->SetParameterName("Length",false);
        fLengthCMD->SetUnitCategory("Length");

        fWidthCMD = new G4UIcmdWithADoubleAndUnit(CommandName("width"),this);
        fWidthCMD->SetGuidance("Set the width of the world.");
        fWidthCMD->SetParameterName("Width",false);
        fWidthCMD->SetUnitCategory("Length");

        fHeightCMD = new G4UIcmdWithADoubleAndUnit(CommandName("height"),this);
        fHeightCMD->SetGuidance("Set the height of the world.");
        fHeightCMD->SetParameterName("Height",false);
        fHeightCMD->SetUnitCategory("Length");
    };

    virtual ~CaptWorldMessenger() {
        delete fLengthCMD;
        delete fWidthCMD;
        delete fHeightCMD;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fLengthCMD) {
            fBuilder->SetLength(fLengthCMD->GetNewDoubleValue(val));
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

void CaptWorldBuilder::Init(void) {
    SetMessenger(new CaptWorldMessenger(this));
    fLength = 10*m;
    fWidth  = fLength;
    fHeight = fLength;

    AddBuilder(new CaptCryostatBuilder("Cryostat",this));

}

CaptWorldBuilder::~CaptWorldBuilder() {};

G4LogicalVolume *CaptWorldBuilder::GetPiece(void) {

    CaptLog("##############################################");
    CaptLog("# CONSTRUCT THE WORLD GEOMETRY ");
    CaptLog("##############################################");

    G4LogicalVolume *logVolume
	= new G4LogicalVolume(new G4Box(GetName(),
                                        fLength/2,  
                                        fWidth/2, 
                                        fHeight/2), 
                              FindMaterial("Air"),
                              GetName());
    logVolume->SetVisAttributes(G4VisAttributes::Invisible);
    
    double floorThickness = 10*cm;
    G4LogicalVolume *logFloor
	= new G4LogicalVolume(new G4Box(GetName()+"/Floor",
                                        fLength/2,
                                        fWidth/2,
                                        floorThickness/2), 
                              FindMaterial("Cement"),
                              GetName()+"/Floor");
    logFloor->SetVisAttributes(G4VisAttributes::Invisible);

    CaptCryostatBuilder& cryo
        = Get<CaptCryostatBuilder>("Cryostat");
    G4LogicalVolume* logCryostat = cryo.GetPiece();
    
    G4ThreeVector cryoCenter = G4ThreeVector(0,0,0)
        - cryo.GetOffset() - cryo.GetTPCOffset();

    new G4PVPlacement(NULL,                   // rotation.
                      cryoCenter,             // position
                      logCryostat,            // logical volume
                      logCryostat->GetName(), // name
                      logVolume,              // mother  volume
                      false,                  // overlapping volume (not used)
                      0,                      // Copy number (zero)
                      Check());               // Check overlaps.

#ifdef SHOW_FLOOR
    G4VisExtent cryoExtent = logCryostat->GetSolid()->GetExtent();
    G4ThreeVector centerFloor(0.0, 0.0,
                              cryoCenter.z() + cryoExtent.GetZmin()
                              - floorThickness/2 - 50*cm);

    new G4PVPlacement(NULL,                // rotation.
                      centerFloor,         // position
                      logFloor,            // logical volume
                      GetName()+"/Floor",  // name
                      logVolume,            // mother  volume
                      false,               // overlapping volume (not used)
                      0,                   // Copy number (zero)
                      Check());            // Check overlaps.

    if (GetVisible()) {
        logFloor->SetVisAttributes(GetColor(logFloor));
    }
#endif

    return logVolume;
}
