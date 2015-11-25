#include "CaptPMTAssemblyBuilder.hh"
#include "CaptPMTBuilder.hh"
#include "DSimBuilder.hh"

#include <DSimLog.hh>

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>

#include <G4Polyhedra.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4SubtractionSolid.hh>

class CaptPMTAssemblyMessenger
    : public DSimBuilderMessenger {
private:
    CaptPMTAssemblyBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fApothemCMD;

public:
    CaptPMTAssemblyMessenger(CaptPMTAssemblyBuilder* c) 
        : DSimBuilderMessenger(c,"Control the drift region geometry."),
          fBuilder(c) {

        fApothemCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("apothem"),this);
        fApothemCMD->SetGuidance("Set the apothem of the PMT assembly.");
        fApothemCMD->SetParameterName("apothem",false);
        fApothemCMD->SetUnitCategory("Length");
    };

    virtual ~CaptPMTAssemblyMessenger() {
        delete fApothemCMD;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fApothemCMD) {
            fBuilder->SetApothem(fApothemCMD->GetNewDoubleValue(val));
        }
        else {
            DSimBuilderMessenger::SetNewValue(cmd,val);
        }
    };
};

void CaptPMTAssemblyBuilder::Init(void) {
    SetMessenger(new CaptPMTAssemblyMessenger(this));
    SetApothem(1037*mm);
    SetPlateThickness(3*mm);
    SetHoleRadius(50*mm);
    SetTPBThickness(6*mm);
    SetTPBSize(50*mm);
    
    SetSensitiveDetector("cryo","segment");

    AddBuilder(new CaptPMTBuilder("PMT",this));
}

CaptPMTAssemblyBuilder::~CaptPMTAssemblyBuilder() {};

double CaptPMTAssemblyBuilder::GetHeight() {
    CaptPMTBuilder& pmt = Get<CaptPMTBuilder>("PMT");
    return pmt.GetBaseLength() + GetTPBThickness();
}

G4ThreeVector CaptPMTAssemblyBuilder::GetOffset() {
    return G4ThreeVector(0,0,0);
}

G4LogicalVolume *CaptPMTAssemblyBuilder::GetPiece(void) {

    double rInner[] = {0.0, 0.0};
    double rOuter[] = {GetApothem(), GetApothem()};
    double zPlane[] = {-GetHeight()/2, GetHeight()/2};

    G4LogicalVolume *logVolume
	= new G4LogicalVolume(new G4Polyhedra(GetName(),
                                              90*degree, 450*degree,
                                              6, 2,
                                              zPlane, rInner, rOuter),
                              FindMaterial("Argon_Liquid"),
                              GetName());
    logVolume->SetVisAttributes(GetColor(logVolume));

    if (GetSensitiveDetector()) {
        logVolume->SetSensitiveDetector(GetSensitiveDetector());
    }

    typedef std::vector< G4ThreeVector > Positions;
    Positions pmtPositions;
    double steps = 5;
    for (double t = 3.14159/steps; t < 2*3.1415; t += 2*3.14159 / steps) {
        double r = GetApothem()/3;
        pmtPositions.push_back(G4ThreeVector(r*std::cos(t),r*std::sin(t),0.0));
    }

    steps = 11;
    for (double t = 3.14159/steps; t < 2*3.1415; t += 2*3.14159 / steps) {
        double r = 2*GetApothem()/3;
        pmtPositions.push_back(G4ThreeVector(r*std::cos(t),r*std::sin(t),0.0));
    }

    // Construct PMT plate
    double platePlane[] = {-GetPlateThickness()/2, GetPlateThickness()/2};
    G4VSolid* shapePlate = new G4Polyhedra("sheet",
                                          90*degree, 450*degree,
                                          6, 2,
                                          platePlane, rInner, rOuter);
    G4VSolid* shapeHole = new G4Tubs("hole",
                                     0.0,
                                     GetHoleRadius(),
                                     0.5+GetPlateThickness()/2.0 , 
                                     0*degree, 360*degree);

    for (Positions::iterator p = pmtPositions.begin();
         p != pmtPositions.end();
         ++p) {
        shapePlate = new G4SubtractionSolid(GetName()+"/Plate",
                                            shapePlate, shapeHole,
                                            0, *p);
    }

    G4LogicalVolume *logPlate
	= new G4LogicalVolume(shapePlate,
                              FindMaterial("FR4"),
                              GetName()+"/Plate");
    logPlate->SetVisAttributes(GetColor(logPlate));

    new G4PVPlacement(NULL,                    // rotation.
                      G4ThreeVector(0.0, 0.0,  // position
                                    (GetHeight()-GetPlateThickness())/2),
                      logPlate,                // logical volume
                      logPlate->GetName(),     // name
                      logVolume,                  // mother  volume
                      false,                   // (not used)
                      0,                       // Copy number (zero)
                      Check());                // Check overlaps.

    /// Put in the TPB plates.
    for (Positions::iterator p = pmtPositions.begin();
         p != pmtPositions.end();
         ++p) {
        G4LogicalVolume* logTPB
            = new G4LogicalVolume(new G4Box(GetName()+"/TPB",
                                            GetTPBSize()/2,
                                            GetTPBSize()/2,
                                            GetTPBThickness()/2),
                                  FindMaterial("Acrylic"),
                                  GetName()+"/TPB");
        logTPB->SetVisAttributes(GetColor(logTPB));
        new G4PVPlacement(NULL,                    // rotation.
                          *p + G4ThreeVector(0.0, 0.0,  // position
                                             (GetHeight()-GetTPBThickness())/2),
                          logTPB,                // logical volume
                          logTPB->GetName(),     // name
                          logVolume,                  // mother  volume
                          false,                   // (not used)
                          0,                       // Copy number (zero)
                          Check());                // Check overlaps.
    }

    /// Put in the PMTs.
    CaptPMTBuilder& pmt = Get<CaptPMTBuilder>("PMT");
    for (Positions::iterator p = pmtPositions.begin();
         p != pmtPositions.end();
         ++p) {
        G4LogicalVolume* logPMT = pmt.GetPiece();
        new G4PVPlacement(NULL,                    // rotation.
                          *p
                          + G4ThreeVector(0.0, 0.0,  // position
                                          (GetHeight()
                                           -GetTPBThickness()
                                           -pmt.GetBaseLength())),
                          logPMT,                // logical volume
                          logPMT->GetName(),     // name
                          logVolume,                  // mother  volume
                          false,                   // (not used)
                          0,                       // Copy number (zero)
                          Check());                // Check overlaps.
    }
    
    return logVolume;
}
