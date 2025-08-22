#include "CaptCryostatBuilder.hh"
#include "CaptImmersedBuilder.hh"
#include "CaptExposedBuilder.hh"
#include "MiniCaptImmersedBuilder.hh"
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
#include <G4Polycone.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

class CaptCryostatMessenger
    : public EDepSim::BuilderMessenger {
private:
    CaptCryostatBuilder* fBuilder;
    G4UIcmdWithADoubleAndUnit* fArgonDepthCMD;
    G4UIcmdWithADoubleAndUnit* fTPCDepthCMD;
    G4UIcmdWithAString* fVesselTypeCMD;

public:
    CaptCryostatMessenger(CaptCryostatBuilder* c)
        : EDepSim::BuilderMessenger(c,"Control the driftRegion geometry."),
          fBuilder(c) {

        fArgonDepthCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("argonDepth"),this);
        fArgonDepthCMD->SetGuidance(
            "Set the distance between the flange and the liquid argon.");
        fArgonDepthCMD->SetParameterName("depth",false);
        fArgonDepthCMD->SetUnitCategory("Length");

        fTPCDepthCMD
            = new G4UIcmdWithADoubleAndUnit(CommandName("tpcDepth"),this);
        fTPCDepthCMD->SetGuidance(
            "Set the distance between the flange and TPC origin.");
        fTPCDepthCMD->SetParameterName("depth",false);
        fTPCDepthCMD->SetUnitCategory("Length");

        fVesselTypeCMD
            = new G4UIcmdWithAString(CommandName("vessel"),this);
        fVesselTypeCMD->SetGuidance(
            "Set the type of vessel to be built.");
        fVesselTypeCMD->SetCandidates("CAPTAIN mCAPTAIN");
    };

    virtual ~CaptCryostatMessenger() {
        delete fArgonDepthCMD;
        delete fTPCDepthCMD;
        delete fVesselTypeCMD;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val) {
        if (cmd==fArgonDepthCMD) {
            fBuilder->SetArgonDepth(
                fArgonDepthCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fTPCDepthCMD) {
            fBuilder->SetTPCDepth(
                fTPCDepthCMD->GetNewDoubleValue(val));
        }
        else if (cmd==fVesselTypeCMD) {
            fBuilder->SetVesselType(val);
        }
        else {
            EDepSim::BuilderMessenger::SetNewValue(cmd,val);
        }
    };
};

void CaptCryostatBuilder::Init(void) {
    SetMessenger(new CaptCryostatMessenger(this));

    SetVesselType("CAPTAIN");

    SetSensitiveDetector("cryo","segment");

    AddBuilder(new CaptImmersedBuilder("Immersed",this));
    AddBuilder(new CaptExposedBuilder("Exposed",this));
    AddBuilder(new MiniCaptImmersedBuilder("mImmersed",this));
    AddBuilder(new MiniCaptExposedBuilder("mExposed",this));
}

CaptCryostatBuilder::~CaptCryostatBuilder() {}

G4ThreeVector CaptCryostatBuilder::GetOffset() {
    return G4ThreeVector(0,0,0);
}

G4ThreeVector CaptCryostatBuilder::GetTPCOffset() {
    return G4ThreeVector(0,0,-GetTPCDepth());
}

void CaptCryostatBuilder::DefineCAPTAINVessel() {
    SetArgonDepth(24*25.4*CLHEP::mm);  // The design spec for CAPTAIN (24 inch).
    SetTPCDepth(GetArgonDepth()+25*CLHEP::mm);   // I made this one up...

    fInnerVessel.clear();
#include "captainInnerVessel.hxx"

    fOuterVessel.clear();
#include "captainOuterVessel.hxx"

    double minZ = 1*CLHEP::km;
    double maxZ = -1*CLHEP::km;
    double maxOuter = 0.0;
    for (Shape::reverse_iterator p = fOuterVessel.rbegin();
         p != fOuterVessel.rend(); ++p) {
        minZ = std::min(p->fZ,minZ);
        maxZ = std::max(p->fZ,maxZ);
        maxOuter = std::max(p->fOuter,maxOuter);
    }
    for (Shape::reverse_iterator p = fInnerVessel.rbegin();
         p != fInnerVessel.rend(); ++p) {
        minZ = std::min(p->fZ,minZ);
        maxZ = std::max(p->fZ,maxZ);
        maxOuter = std::max(p->fOuter,maxOuter);
    }
    maxOuter += 1*CLHEP::mm;
    fVesselEnvelope.push_back(Point(minZ,0.0,maxOuter));
    fVesselEnvelope.push_back(Point(maxZ,0.0,maxOuter));

}

void CaptCryostatBuilder::DefineMiniCAPTAINVessel() {
    SetArgonDepth(9*25.4*CLHEP::mm);  // The design spec for CAPTAIN (24 inch).
    SetTPCDepth(GetArgonDepth()+25*CLHEP::mm);   // I made this one up...

    fInnerVessel.clear();
#include "miniCaptainInnerVessel.hxx"

    fOuterVessel.clear();
#include "miniCaptainOuterVessel.hxx"

    double minZ = 1*CLHEP::km;
    double maxZ = -1*CLHEP::km;
    double maxOuter = 0.0;
    for (Shape::reverse_iterator p = fOuterVessel.rbegin();
         p != fOuterVessel.rend(); ++p) {
        minZ = std::min(p->fZ,minZ);
        maxZ = std::max(p->fZ,maxZ);
        maxOuter = std::max(p->fOuter,maxOuter);
    }
    for (Shape::reverse_iterator p = fInnerVessel.rbegin();
         p != fInnerVessel.rend(); ++p) {
        minZ = std::min(p->fZ,minZ);
        maxZ = std::max(p->fZ,maxZ);
        maxOuter = std::max(p->fOuter,maxOuter);
    }
    maxOuter += 1*CLHEP::mm;
    fVesselEnvelope.push_back(Point(minZ,0.0,maxOuter));
    fVesselEnvelope.push_back(Point(maxZ,0.0,maxOuter));

}

G4LogicalVolume *CaptCryostatBuilder::GetPiece(void) {

    if (fVesselType == "CAPTAIN") {
        DefineCAPTAINVessel();
    }
    else if (fVesselType == "mCAPTAIN") {
        DefineMiniCAPTAINVessel();
    }
    else {
        std::cout << "Undefine vessel type: " << fVesselType << std::endl;
        std::exit(0);
    }

    std::vector<double> conePlanes;
    std::vector<double> coneMax;
    std::vector<double> coneMin;

    ////////////////////////////////////////////////////////
    // Define the envelope to contain the vessel.
    ////////////////////////////////////////////////////////
    for (Shape::reverse_iterator p = fVesselEnvelope.rbegin();
         p != fVesselEnvelope.rend();
         ++p) {
        conePlanes.push_back(- p->fZ);
        coneMin.push_back(0.0);
        coneMax.push_back(p->fOuter+10*CLHEP::cm);
    }

    G4LogicalVolume* logVolume
        = new G4LogicalVolume(
            new G4Polycone(GetName(),
                           0*CLHEP::degree, 360*CLHEP::degree,
                           conePlanes.size(),
                           conePlanes.data(),
                           coneMin.data(),
                           coneMax.data()),
            FindMaterial("Air"),
            GetName());
    logVolume->SetVisAttributes(G4VisAttributes::Invisible);

#define BUILD_OUTER_VESSEL
#ifdef BUILD_OUTER_VESSEL
    ////////////////////////////////////////////////////////
    // Define the outer vessel.
    ////////////////////////////////////////////////////////
    conePlanes.clear();
    coneMax.clear();
    coneMin.clear();
    for (Shape::reverse_iterator p = fOuterVessel.rbegin();
         p != fOuterVessel.rend();
         ++p) {
        conePlanes.push_back(- p->fZ);
        coneMin.push_back(p->fInner);
        coneMax.push_back(p->fOuter);
    }

    G4LogicalVolume* logOuterVessel
        = new G4LogicalVolume(
            new G4Polycone(GetName()+"/OuterVessel",
                           0*CLHEP::degree, 360*CLHEP::degree,
                           conePlanes.size(),
                           conePlanes.data(),
                           coneMin.data(),
                           coneMax.data()),
            FindMaterial("SS_304"),
            GetName()+"/OuterVessel");
    logOuterVessel->SetVisAttributes(GetColor(logOuterVessel));

    new G4PVPlacement(NULL,                // rotation.
                      G4ThreeVector(0,0,0),
                      logOuterVessel,           // logical volume
                      logOuterVessel->GetName(), // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.
#endif

#define BUILD_INNER_VESSEL
#ifdef BUILD_INNER_VESSEL
    ////////////////////////////////////////////////////////
    // Define the inner vessel.
    ////////////////////////////////////////////////////////
    conePlanes.clear();
    coneMax.clear();
    coneMin.clear();
    for (Shape::reverse_iterator p = fInnerVessel.rbegin();
         p != fInnerVessel.rend();
         ++p) {
        conePlanes.push_back(- p->fZ);
        coneMin.push_back(p->fInner);
        coneMax.push_back(p->fOuter);
    }

    G4LogicalVolume* logInnerVessel
        = new G4LogicalVolume(
            new G4Polycone(GetName()+"/InnerVessel",
                           0*CLHEP::degree, 360*CLHEP::degree,
                           conePlanes.size(),
                           conePlanes.data(),
                           coneMin.data(), coneMax.data()),
            FindMaterial("SS_304"),
            GetName()+"/InnerVessel");
    logInnerVessel->SetVisAttributes(GetColor(logInnerVessel));

    new G4PVPlacement(NULL,                // rotation.
                      G4ThreeVector(0,0,0),
                      logInnerVessel,           // logical volume
                      logInnerVessel->GetName(), // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.

    ////////////////////////////////////////////////////////
    // Define the liquid volume.
    ////////////////////////////////////////////////////////
    conePlanes.clear();
    coneMax.clear();
    coneMin.clear();
    for (Shape::reverse_iterator p = fInnerVessel.rbegin();
         p != fInnerVessel.rend();
         ++p) {
        if (p->fZ < GetArgonDepth()) continue;
        conePlanes.push_back(- p->fZ);
        coneMin.push_back(0.0);
        coneMax.push_back(p->fInner);
    }
    if (conePlanes.back() < -GetArgonDepth()) {
        conePlanes.push_back(- GetArgonDepth());
        coneMin.push_back(0.0);
        coneMax.push_back(coneMax.back());
    }

    fLiquidVolume
        = new G4LogicalVolume(
            new G4Polycone(GetName()+"/Liquid",
                           0*CLHEP::degree, 360*CLHEP::degree,
                           conePlanes.size(),
                           conePlanes.data(),
                           coneMin.data(), coneMax.data()),
            FindMaterial("Argon_Liquid"),
            GetName()+"/Liquid");
    fLiquidVolume->SetVisAttributes(GetColor(fLiquidVolume));

    new G4PVPlacement(NULL,                // rotation.
                      G4ThreeVector(0,0,0),
                      fLiquidVolume,           // logical volume
                      fLiquidVolume->GetName()+"/Liquid", // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.

    if (fVesselType == "CAPTAIN") {
        CaptImmersedBuilder& immersed = Get<CaptImmersedBuilder>("Immersed");
        G4LogicalVolume* logImmersed = immersed.GetPiece();
        G4ThreeVector p = GetTPCOffset() - immersed.GetOffset();
        new G4PVPlacement(NULL,                   // rotation.
                          p,                      // position
                          logImmersed,            // logical volume
                          logImmersed->GetName(), // name
                          fLiquidVolume,          // mother  volume
                          false,                  // (not used)
                          0,                      // Copy number (zero)
                          Check());               // Check overlaps.
    }
    else if (fVesselType == "mCAPTAIN") {
        MiniCaptImmersedBuilder& immersed
            = Get<MiniCaptImmersedBuilder>("mImmersed");
        G4LogicalVolume* logImmersed = immersed.GetPiece();
        G4ThreeVector p = GetTPCOffset() - immersed.GetOffset();
        new G4PVPlacement(NULL,                   // rotation.
                          p,                      // position
                          logImmersed,            // logical volume
                          logImmersed->GetName(), // name
                          fLiquidVolume,          // mother  volume
                          false,                  // (not used)
                          0,                      // Copy number (zero)
                          Check());               // Check overlaps.
    }
    else {
        std::cout << "Undefined immersed volume" << std::endl;
        std::exit(0);
    }
#endif

#ifdef BUILD_ULLAGE
    ////////////////////////////////////////////////////////
    // Define the ullage volume.
    ////////////////////////////////////////////////////////
    conePlanes.clear();
    coneMax.clear();
    coneMin.clear();
    for (Shape::reverse_iterator p = fInnerVessel.rbegin();
         p != fInnerVessel.rend();
         ++p) {
        if (p->fZ >= GetArgonDepth()-1*CLHEP::mm) continue;
        if (conePlanes.empty() ) {
            conePlanes.push_back(-GetArgonDepth()+1*CLHEP::um);
            coneMin.push_back(0.0);
            coneMax.push_back(p->fInner);
        }
        conePlanes.push_back(- p->fZ);
        coneMin.push_back(0.0);
        coneMax.push_back(p->fInner);
    }

    fUllageVolume
        = new G4LogicalVolume(
            new G4Polycone(GetName()+"/Ullage",
                           0*CLHEP::degree, 360*CLHEP::degree,
                           conePlanes.size(),
                           conePlanes.data(),
                           coneMin.data(),
                           coneMax.data()),
            FindMaterial("Argon_Gas"),
            GetName()+"/Ullage");
    fUllageVolume->SetVisAttributes(GetColor(fUllageVolume));

    new G4PVPlacement(NULL,                // rotation.
                      G4ThreeVector(0,0,0),
                      fUllageVolume,           // logical volume
                      fUllageVolume->GetName(), // name
                      logVolume,                // mother  volume
                      false,                    // (not used)
                      0,                        // Copy number (zero)
                      Check());                 // Check overlaps.

    if (fVesselType == "CAPTAIN") {
        CaptExposedBuilder& exposed = Get<CaptExposedBuilder>("Exposed");
        G4LogicalVolume* logExposed = exposed.GetPiece();
        G4ThreeVector p(0.0,0.0,-GetArgonDepth());
        p -= exposed.GetOffset();
        new G4PVPlacement(NULL,                   // rotation.
                          p,                      // position
                          logExposed,            // logical volume
                          logExposed->GetName(), // name
                          fUllageVolume,          // mother  volume
                          false,                  // (not used)
                          0,                      // Copy number (zero)
                          Check());               // Check overlaps.
    }
    else if (fVesselType == "mCAPTAIN") {
        MiniCaptExposedBuilder& exposed
            = Get<MiniCaptExposedBuilder>("mExposed");
        G4LogicalVolume* logExposed = exposed.GetPiece();
        if (logExposed) {
            G4ThreeVector p(0.0,0.0,-GetArgonDepth());
            p -= exposed.GetOffset();
            new G4PVPlacement(NULL,                   // rotation.
                              p,                      // position
                              logExposed,            // logical volume
                              logExposed->GetName(), // name
                              fUllageVolume,          // mother  volume
                              false,                  // (not used)
                              0,                      // Copy number (zero)
                              Check());               // Check overlaps.
        }
    }
    else {
        std::cout << "Undefined exposed volume" << std::endl;
        std::exit(0);
    }
#endif

    return logVolume;
}
