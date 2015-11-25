#include "G4ios.hh"
#include "G4NistManager.hh"
#include "G4StableIsotopes.hh"

#include "G4Isotope.hh"
#include "G4Material.hh"
#include "G4UnitsTable.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4VPersistencyManager.hh"

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

#include "DSimUserDetectorConstruction.hh"
#include "DSimDetectorMessenger.hh"
#include "DSimRootGeometryManager.hh"
#include "captain/CaptWorldBuilder.hh"

#include "G4RegionStore.hh"

#include <DSimLog.hh>
 
DSimUserDetectorConstruction::DSimUserDetectorConstruction() {

    fDetectorMessenger = new DSimDetectorMessenger(this);
    fWorldBuilder = new CaptWorldBuilder("/Captain",this);
    fDefaultMaterial = NULL;
    fValidateGeometry = false;
}
 
DSimUserDetectorConstruction::~DSimUserDetectorConstruction() { 
    delete fDetectorMessenger;
    delete fWorldBuilder;
}
 
G4VPhysicalVolume* DSimUserDetectorConstruction::Construct() {
    DefineMaterials();
    return ConstructDetector();
}

void DSimUserDetectorConstruction::DefineMaterials() {
    DSimRootGeometryManager* geoMan = DSimRootGeometryManager::Get();
    G4double a, density;
    G4String name, symbol;
    G4double temperature, pressure;
    G4int nel, ncomponents, natoms;
    G4double fractionmass;

    G4NistManager* nistMan = G4NistManager::Instance();

    G4Element* elH = nistMan->FindOrBuildElement(1);

    G4Element* elB = nistMan->FindOrBuildElement(5);

    G4Element* elC = nistMan->FindOrBuildElement(6);

    G4Element* elN = nistMan->FindOrBuildElement(7);

    G4Element* elO = nistMan->FindOrBuildElement(8);

    G4Element* elF = nistMan->FindOrBuildElement(9);

    G4Element* elNa = nistMan->FindOrBuildElement(11);

    G4Element* elAl = nistMan->FindOrBuildElement(13);

    G4Element* elSi = nistMan->FindOrBuildElement(14);

    G4Element* elCl = nistMan->FindOrBuildElement(17);

    G4Element* elAr = nistMan->FindOrBuildElement(18);

    G4Element* elTi = nistMan->FindOrBuildElement(22);

    G4Element* elFe = nistMan->FindOrBuildElement(26);

    G4Element* elCo = nistMan->FindOrBuildElement(27);

    G4Element* elCu = nistMan->FindOrBuildElement(29);

    G4Element* elZn = nistMan->FindOrBuildElement(30);

    G4Element* elSn = nistMan->FindOrBuildElement(50);

    G4Element* elPb = nistMan->FindOrBuildElement(82);

    //Air
    G4Material* air 
        = new G4Material(name="Air",
                         density = 1.29*mg/cm3,
                         nel=2,
                         kStateGas,
                         temperature = 293.15*kelvin,
                         pressure=1*atmosphere);
    air->AddElement(elN, fractionmass = 70*perCent);
    air->AddElement(elO, fractionmass = 30*perCent);
    geoMan->SetDrawAtt(air,kGray+3,0.01);

    // This is the default material.
    fDefaultMaterial = air;

    //Earth
    density = 2.15*g/cm3;
    G4Material* earth 
        = new G4Material(name="Earth", 
                         density = 2.15*g/cm3,
                         nel=2,
                         kStateSolid,
                         temperature = 293.15*kelvin,
                         pressure=1*atmosphere);
    earth->AddElement(elSi, natoms=1);
    earth->AddElement(elO, natoms=2);
    geoMan->SetDrawAtt(earth,49,0.2);
    
    //Cement
    G4Material* cement 
        = new G4Material(name="Cement",
                         density = 2.5*g/cm3,
                         nel=2,
                         kStateSolid,
                         temperature = 293.15*kelvin,
                         pressure=1*atmosphere);
    cement->AddElement(elSi, natoms=1);
    cement->AddElement(elO, natoms=2);
    geoMan->SetDrawAtt(cement,kGray,0.2);

    // The usual stainless steel (SS_304).
    density = 8.0*g/cm3;
    G4Material* SS_304 = new G4Material(name="SS_304",
                                       density = 8.0*g/cm3,
                                       nel=3,
                                       kStateSolid,
                                       temperature = 293.15*kelvin,
                                       pressure=1*atmosphere);
    SS_304->AddElement(elC,  fractionmass =  4*perCent);
    SS_304->AddElement(elFe, fractionmass = 88*perCent);
    SS_304->AddElement(elCo, fractionmass =  8*perCent);
    geoMan->SetDrawAtt(SS_304,kBlue-10,0.05);

    // Argon Gas
    G4Material* argon =  new G4Material(name="Argon_Gas",
                                        density = 1.66*mg/cm3,
                                        nel=1,
                                        kStateGas,
                                        temperature = 87.3*kelvin,
                                        pressure=1*atmosphere);
    argon->AddElement(elAr, natoms=1);
    geoMan->SetDrawAtt(argon,kMagenta-10,0.1);

    // Liquid Argon
    G4Material* LAr =  new G4Material(name="Argon_Liquid",
                                      density = 1.3954*g/cm3,
                                      nel=1,
                                      kStateLiquid,
                                      temperature = 87.3*kelvin,
                                      pressure=1*atmosphere);
    LAr->AddElement(elAr, natoms=1);

    // Set up liquid argon for NEST.
    G4MaterialPropertiesTable *LArMatProps = new G4MaterialPropertiesTable();
    LArMatProps->AddConstProperty("ELECTRICFIELD",500*volt/cm);
    LArMatProps->AddConstProperty("TOTALNUM_INT_SITES",-1);
    LAr->SetMaterialPropertiesTable(LArMatProps);

    geoMan->SetDrawAtt(LAr,kCyan-9,0.1);

    // The CAPTAIN TPC wire.
    G4Material *wire = new G4Material(name="Captain_Wire",
                                      density = 8.96*g/cm3,
                                      nel=1,
                                      kStateSolid,
                                      temperature = 87.3*kelvin,
                                      pressure=1*atmosphere);
    wire->AddElement(elCu, natoms=1);
    geoMan->SetDrawAtt(wire,kOrange+1,1.0);

    // Glass - 
    G4Material* glass
        = new G4Material(name="Glass", 
                         density = 2.70*g/cm3,
                         nel=4);
    glass->AddElement(elO,53.9*perCent);
    glass->AddElement(elSi,38.4*perCent);
    glass->AddElement(elB,3.8*perCent);
    glass->AddElement(elNa,3.8*perCent);
    geoMan->SetDrawAtt(glass,kBlue+1,0.3);

    // G10 - by volume 57% glass, 43% epoxy (CH2)
    G4Material* g10
        = new G4Material(name="G10", 
                         density = 1.70*g/cm3,
                         nel=6);
    g10->AddElement(elH,6.2*perCent);
    g10->AddElement(elC,36.8*perCent);
    g10->AddElement(elO,30.7*perCent);
    g10->AddElement(elSi,21.9*perCent);
    g10->AddElement(elB,2.2*perCent);
    g10->AddElement(elNa,2.2*perCent);
    geoMan->SetDrawAtt(g10,kGreen+1,0.75);

    // FR4 - Approximated by the composition of G10.  The density is from
    // Wikipedia.
    G4Material* fr4
        = new G4Material(name="FR4", 
                         density = 1850*kg/m3,
                         nel=6);
    fr4->AddElement(elH,6.2*perCent);
    fr4->AddElement(elC,36.8*perCent);
    fr4->AddElement(elO,30.7*perCent);
    fr4->AddElement(elSi,21.9*perCent);
    fr4->AddElement(elB,2.2*perCent);
    fr4->AddElement(elNa,2.2*perCent);
    geoMan->SetDrawAtt(fr4,kGreen+1,0.75);

    // FR4_Copper - Approximated by the composition of G10 plus copper.  The
    // copper is from the cladding, but is approximated as spread through the
    // FR4.  The density is from Wikipedia.
    G4Material* fr4Copper
        = new G4Material(name="FR4_Copper", 
                         density = 1850*kg/m3,
                         nel=7);
    double cuFrac = 3*perCent;
    double fr4Frac = 1.0 - cuFrac;
    fr4Copper->AddElement(elH,6.2*perCent*fr4Frac);
    fr4Copper->AddElement(elC,36.8*perCent*fr4Frac);
    fr4Copper->AddElement(elO,30.7*perCent*fr4Frac);
    fr4Copper->AddElement(elSi,21.9*perCent*fr4Frac);
    fr4Copper->AddElement(elB,2.2*perCent*fr4Frac);
    fr4Copper->AddElement(elNa,2.2*perCent*fr4Frac);
    fr4Copper->AddElement(elCu,cuFrac);
    geoMan->SetDrawAtt(fr4Copper,kYellow-6,0.9);

    // Acrylic - Approximated by the composition of the Acrylic used to hold
    // the TPB..  The density is from Wikipedia.
    G4Material* acrylic
        = new G4Material(name="Acrylic", 
                         density = 1189*kg/m3,
                         nel=3);
    acrylic->AddElement(elH,53.4*perCent*fr4Frac);
    acrylic->AddElement(elO,13.3*perCent*fr4Frac);
    acrylic->AddElement(elC,33.3*perCent);
    geoMan->SetDrawAtt(acrylic,kAzure+6,0.9);

    // Print all the materials defined.
    DSimLog(*(G4Material::GetMaterialTable()));
}

G4Element* DSimUserDetectorConstruction::DefineElement(G4String name,
                                                        G4String symbol,
                                                        G4double z) {
    static G4StableIsotopes theDefaultIsotopes;

    G4int Z = G4int(z);
    G4int ncomponents = theDefaultIsotopes.GetNumberOfIsotopes(Z);
    G4Element* el = new G4Element(name, symbol, ncomponents);
    G4int first = theDefaultIsotopes.GetFirstIsotope(Z);
    for (G4int i=0; i<ncomponents; i++)
    {
        G4int A = theDefaultIsotopes.GetIsotopeNucleonCount(first+i);
        std::ostringstream os; os << symbol << A;
        G4Isotope* is = new G4Isotope(name=os.str(), Z, A, A*g/mole);
        G4double abundance = theDefaultIsotopes.GetAbundance(first+i);
        el->AddIsotope(is, abundance*perCent);
    }
    return el;
}

G4VPhysicalVolume* DSimUserDetectorConstruction::ConstructDetector() {

    // Create a region outside of the detector to define cuts.
    G4RegionStore::GetInstance()->FindOrCreateRegion("hallRegion");
#ifdef USE_PAI
    // Create a region for the PAI Model.
    G4RegionStore::GetInstance()->FindOrCreateRegion("driftRegion");
#endif

    G4String name = fWorldBuilder->GetName();
    G4LogicalVolume *vol = fWorldBuilder->GetPiece();

    //------------------------------ 
    // World
    //------------------------------ 
    //  Must place the World Physical volume unrotated at (0,0,0).
    G4VPhysicalVolume* physWorld 
        = new G4PVPlacement(0,               // no rotation
                            G4ThreeVector(), // position (0,0,0)
                            name, // name
                            vol,// logical volume
                            0,               // mother  volume
                            false,           // no boolean operations
                            0);

    G4RunManager::GetRunManager()->DefineWorldVolume(physWorld);

    DSimRootGeometryManager::Get()->Update(physWorld,fValidateGeometry);

    G4VPersistencyManager *pMan
        = G4VPersistencyManager::GetPersistencyManager();
    if (pMan) pMan->Store(physWorld);

    return physWorld;
}

void DSimUserDetectorConstruction::UpdateGeometry() {
    // Initialize the run manager.  This sets everything in motion.
    G4RunManager::GetRunManager()->Initialize();
}
