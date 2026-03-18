#include "EDepSimLog.hh"

#include <G4Version.hh>

#include <G4ios.hh>
#include <G4NistManager.hh>
#include <G4StableIsotopes.hh>

#include <G4Isotope.hh>
#include <G4Material.hh>
#include <G4UnitsTable.hh>
#include <G4RunManager.hh>

#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include "EDepSimRootGeometryManager.hh"
#include "CaptMaterialDefinitions.hh"

void EDepSim::CaptMaterialDefinitions::DefineMaterials() {
    EDepSim::RootGeometryManager* geoMan = EDepSim::RootGeometryManager::Get();
    G4double density;
    G4String name, symbol;
    G4double temperature, pressure;
    G4int nel, natoms;
    G4double fractionmass;

    G4NistManager* nistMan = G4NistManager::Instance();

    G4Element* elH = nistMan->FindOrBuildElement(1);

    G4Element* elB = nistMan->FindOrBuildElement(5);

    G4Element* elC = nistMan->FindOrBuildElement(6);

    G4Element* elN = nistMan->FindOrBuildElement(7);

    G4Element* elO = nistMan->FindOrBuildElement(8);

    // G4Element* elF = nistMan->FindOrBuildElement(9);

    G4Element* elNa = nistMan->FindOrBuildElement(11);

    // G4Element* elAl = nistMan->FindOrBuildElement(13);

    G4Element* elSi = nistMan->FindOrBuildElement(14);

    // G4Element* elCl = nistMan->FindOrBuildElement(17);

    G4Element* elAr = nistMan->FindOrBuildElement(18);

    // G4Element* elTi = nistMan->FindOrBuildElement(22);

    G4Element* elFe = nistMan->FindOrBuildElement(26);

    G4Element* elCo = nistMan->FindOrBuildElement(27);

    G4Element* elCu = nistMan->FindOrBuildElement(29);

    // G4Element* elZn = nistMan->FindOrBuildElement(30);

    // G4Element* elSn = nistMan->FindOrBuildElement(50);

    // G4Element* elPb = nistMan->FindOrBuildElement(82);

    std::vector<G4double> photWavelength;
    std::vector<G4double> photEnergy;
    for (double w = 750.0*nm; w > 199.0*nm; w -= 50.0*nm) {
        photWavelength.push_back(w);
        photEnergy.push_back((twopi*hbarc)/w);
    }

    // Air -- Defined to have a default material.
    G4Material* air
        = new G4Material(name="Air",
                         density = 1.29*CLHEP::mg/CLHEP::cm3,
                         nel=2,
                         kStateGas,
                         temperature = 293.15*CLHEP::kelvin,
                         pressure=1*CLHEP::atmosphere);
    air->AddElement(elN, fractionmass = 70*CLHEP::perCent);
    air->AddElement(elO, fractionmass = 30*CLHEP::perCent);
    geoMan->SetDrawAtt(air,kGray+3,0.01);

    G4MaterialPropertiesTable *airMPT = new G4MaterialPropertiesTable();
    air->SetMaterialPropertiesTable(airMPT);

    // Set up argon gas for optical photons (these are very bogus values
    // for testing.  Chosen to test optical processses.)
    std::vector<G4double> airRIndex{
        1.0001, 1.0001, // 750, 700
        1.0001, 1.0001, // 650, 600
        1.0001, 1.0001, // 550, 500
        1.0001, 1.0001, // 450, 400
        1.0001, 1.0001, // 350, 300
        1.0001, 1.0001, // 250, 200
    };
    EDepSimError("Set RINDEX " << photEnergy.size()
                 << " " << airRIndex.size());
    airMPT->AddProperty("RINDEX",photEnergy,airRIndex);
    std::vector<G4double> airAbsLength{
        10000*cm, 10000*cm, // 750, 700
        10000*cm, 10000*cm, // 650, 600
        10000*cm, 10000*cm, // 550, 500
        10000*cm, 10000*cm, // 450, 400
        10000*cm, 10000*cm, // 350, 300
        10000*cm, 10000*cm, // 250, 200
    };
    airMPT->AddProperty("ABSLENGTH",photEnergy,airAbsLength);

    //Earth
    density = 2.15*CLHEP::g/CLHEP::cm3;
    G4Material* earth
        = new G4Material(name="Earth",
                         density = 2.15*CLHEP::g/CLHEP::cm3,
                         nel=2,
                         kStateSolid,
                         temperature = 293.15*CLHEP::kelvin,
                         pressure=1*CLHEP::atmosphere);
    earth->AddElement(elSi, natoms=1);
    earth->AddElement(elO, natoms=2);
    geoMan->SetDrawAtt(earth,49,0.2);

    //Cement
    G4Material* cement
        = new G4Material(name="Cement",
                         density = 2.5*CLHEP::g/CLHEP::cm3,
                         nel=2,
                         kStateSolid,
                         temperature = 293.15*CLHEP::kelvin,
                         pressure=1*CLHEP::atmosphere);
    cement->AddElement(elSi, natoms=1);
    cement->AddElement(elO, natoms=2);
    geoMan->SetDrawAtt(cement,kGray,0.2);

    // The usual stainless steel (SS_304).
    density = 8.0*CLHEP::g/CLHEP::cm3;
    G4Material* SS_304 = new G4Material(name="SS_304",
                                       density = 8.0*CLHEP::g/CLHEP::cm3,
                                       nel=3,
                                       kStateSolid,
                                       temperature = 293.15*CLHEP::kelvin,
                                       pressure=1*CLHEP::atmosphere);
    SS_304->AddElement(elC,  fractionmass =  4*CLHEP::perCent);
    SS_304->AddElement(elFe, fractionmass = 88*CLHEP::perCent);
    SS_304->AddElement(elCo, fractionmass =  8*CLHEP::perCent);
    geoMan->SetDrawAtt(SS_304,kBlue-10,0.05);

    // Argon Gas
    G4Material* argon =  new G4Material(name="Argon_Gas",
                                        density = 1.66*CLHEP::mg/CLHEP::cm3,
                                        nel=1,
                                        kStateGas,
                                        temperature = 87.3*CLHEP::kelvin,
                                        pressure=1*CLHEP::atmosphere);
    argon->AddElement(elAr, natoms=1);
    geoMan->SetDrawAtt(argon,kMagenta-10,0.1);

    G4MaterialPropertiesTable *argonMPT = new G4MaterialPropertiesTable();
    argon->SetMaterialPropertiesTable(argonMPT);

    // Set up argon gas for optical photons (these are very bogus values
    // for testing.  Chosen to test optical processses.)
    std::vector<G4double> argonRIndex{
        1.0001, 1.0001, // 750, 700
        1.0001, 1.0001, // 650, 600
        1.0001, 1.0001, // 550, 500
        1.0001, 1.0001, // 450, 400
        1.0001, 1.0001, // 350, 300
        1.0001, 1.0001, // 250, 200
    };
    argonMPT->AddProperty("RINDEX",photEnergy,argonRIndex);
    std::vector<G4double> argonAbsLength{
        10000*cm, 10000*cm, // 750, 700
        10000*cm, 10000*cm, // 650, 600
        10000*cm, 10000*cm, // 550, 500
        10000*cm, 10000*cm, // 450, 400
        10000*cm, 10000*cm, // 350, 300
        10000*cm, 10000*cm, // 250, 200
    };
    argonMPT->AddProperty("ABSLENGTH",photEnergy,argonAbsLength);

    // Setup a completely bogus WLS model for Argon_Gas.  This is ONLY there
    // to test the WLS process.

    // Liquid Argon
    const double LArDensity = 1.3954*g/cm3;
    // Provide a default Birks constant for LAr in case Doke-Birks is off.
    // This is loosely based off of a mentioned ICARUS value for testing
    // G4EmSaturation, but not chased to the original reference (don't use it
    // for physics).
    const double LArBirks = 0.0486*g/cm2/MeV/LArDensity;

    G4Material* LAr =  new G4Material(name="Argon_Liquid",
                                      density = 1.3954*CLHEP::g/CLHEP::cm3,
                                      nel=1,
                                      kStateLiquid,
                                      temperature = 87.3*CLHEP::kelvin,
                                      pressure=1*CLHEP::atmosphere);
    LAr->AddElement(elAr, natoms=1);
    LAr->GetIonisation()->SetBirksConstant(LArBirks);

    // Set up liquid argon material properties
    G4MaterialPropertiesTable *LArMPT = new G4MaterialPropertiesTable();
    LAr->SetMaterialPropertiesTable(LArMPT);

    // Set up liquid argon for optical photons (BOGUS!! Chosen to test optical
    // processses, and do not match argon).
    std::vector<G4double> LArRIndex{
        1.23, 1.23, // 750, 700
        1.23, 1.23, // 650, 600
        1.23, 1.23, // 550, 500
        1.23, 1.23, // 450, 400
        1.23, 1.23, // 350, 300
        1.23, 1.23, // 250, 200
    };
    LArMPT->AddProperty("RINDEX",photEnergy,LArRIndex);
    std::vector<G4double> LArAbsLength{
        3000*cm, 3000*cm, // 750, 700
        3000*cm, 3000*cm, // 650, 600
        3000*cm, 3000*cm, // 550, 500
        3000*cm, 3000*cm, // 450, 400
        3000*cm, 3000*cm, // 350, 300
        3000*cm, 3000*cm, // 250, 200
    };
    LArMPT->AddProperty("ABSLENGTH",photEnergy,LArAbsLength);

    // Set the LAr scintillator properties (BOGUS!! Chosen to test optical
    // processes and do not match argon)
    LArMPT->AddConstProperty("SCINTILLATIONYIELD",29000); // quanta/MeV
    LArMPT->AddConstProperty("RESOLUTIONSCALE",1.0); // Required: should be 1.0
    std::vector<G4double> scintillationEmision1{2.034 * eV, 4.136 * eV};
    std::vector<G4double> scintillationComponent1{1.0, 1.0};
    LArMPT->AddProperty("SCINTILLATIONCOMPONENT1",
                        scintillationEmision1,
                        scintillationComponent1);
    LArMPT->AddConstProperty("SCINTILLATIONYIELD1",1.0); // comp1 fraction
    LArMPT->AddConstProperty("SCINTILLATIONRISETIME1", 1. * ns);
    LArMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 3. * ns);

    // Set up liquid argon for NEST.
#if G4VERSION_NUMBER < 1100
    LArMPT->AddConstProperty("ELECTRICFIELD",500*CLHEP::volt/CLHEP::cm);
    LArMPT->AddConstProperty("TOTALNUM_INT_SITES",-1);
#else
    LArMPT->AddConstProperty("ELECTRICFIELD",500*CLHEP::volt/CLHEP::cm,true);
    LArMPT->AddConstProperty("TOTALNUM_INT_SITES",-1,true);
#endif

    geoMan->SetDrawAtt(LAr,kCyan-9,0.1);

    // The CAPTAIN TPC wire.
    G4Material *wire = new G4Material(name="Captain_Wire",
                                      density = 8.96*CLHEP::g/CLHEP::cm3,
                                      nel=1,
                                      kStateSolid,
                                      temperature = 87.3*CLHEP::kelvin,
                                      pressure=1*CLHEP::atmosphere);
    wire->AddElement(elCu, natoms=1);
    geoMan->SetDrawAtt(wire,kOrange+1,1.0);

    // Glass -
    G4Material* glass
        = new G4Material(name="Glass",
                         density = 2.70*CLHEP::g/CLHEP::cm3,
                         nel=4);
    glass->AddElement(elO,53.9*CLHEP::perCent);
    glass->AddElement(elSi,38.4*CLHEP::perCent);
    glass->AddElement(elB,3.8*CLHEP::perCent);
    glass->AddElement(elNa,3.8*CLHEP::perCent);
    geoMan->SetDrawAtt(glass,kBlue+1,0.3);

    // Set up glass material properties
    G4MaterialPropertiesTable *glassMPT = new G4MaterialPropertiesTable();
    glass->SetMaterialPropertiesTable(glassMPT);

    // Set up glass for optical photons (these are bogus values for
    // testing.  Chosen to test optical processses)
    std::vector<G4double> glassRIndex{
        1.55, 1.55, // 750, 700
        1.55, 1.55, // 650, 600
        1.55, 1.55, // 550, 500
        1.55, 1.55, // 450, 400
        1.55, 1.55, // 350, 300
        1.55, 1.55, // 250, 200
    };
    glassMPT->AddProperty("RINDEX",photEnergy,glassRIndex);
    std::vector<G4double> glassAbsLength{
        300*cm, 300*cm, // 750, 700
        300*cm, 300*cm, // 650, 600
        300*cm, 300*cm, // 550, 500
        300*cm, 300*cm, // 450, 400
        30*cm, 3*cm, // 350, 300
        3*cm, 3*cm, // 250, 200
    };
    glassMPT->AddProperty("ABSLENGTH",photEnergy,glassAbsLength);

    // G10 - by volume 57% glass, 43% epoxy (CH2)
    G4Material* g10
        = new G4Material(name="G10",
                         density = 1.70*CLHEP::g/CLHEP::cm3,
                         nel=6);
    g10->AddElement(elH,6.2*CLHEP::perCent);
    g10->AddElement(elC,36.8*CLHEP::perCent);
    g10->AddElement(elO,30.7*CLHEP::perCent);
    g10->AddElement(elSi,21.9*CLHEP::perCent);
    g10->AddElement(elB,2.2*CLHEP::perCent);
    g10->AddElement(elNa,2.2*CLHEP::perCent);
    geoMan->SetDrawAtt(g10,kGreen+1,0.75);

    // Set up g10 material properties
    G4MaterialPropertiesTable *g10MPT = new G4MaterialPropertiesTable();
    g10->SetMaterialPropertiesTable(g10MPT);

    // Set up g10 for optical photons (these are bogus values for
    // testing.  Chosen to test optical processses)
    std::vector<G4double> g10RIndex{
        1.55, 1.55, // 750, 700
        1.55, 1.55, // 650, 600
        1.55, 1.55, // 550, 500
        1.55, 1.55, // 450, 400
        1.55, 1.55, // 350, 300
        1.55, 1.55, // 250, 200
    };
    g10MPT->AddProperty("RINDEX",photEnergy,g10RIndex);
    std::vector<G4double> g10AbsLength{
        300*um, 300*um, // 750, 700
        300*um, 300*um, // 650, 600
        300*um, 300*um, // 550, 500
        300*um, 300*um, // 450, 400
        300*um, 300*um, // 350, 300
        300*um, 300*um, // 250, 200
    };
    g10MPT->AddProperty("ABSLENGTH",photEnergy,g10AbsLength);

    // FR4 - Approximated by the composition of G10.  The density is from
    // Wikipedia.
    G4Material* fr4
        = new G4Material(name="FR4",
                         density = 1850*CLHEP::kg/CLHEP::m3,
                         nel=6);
    fr4->AddElement(elH,6.2*CLHEP::perCent);
    fr4->AddElement(elC,36.8*CLHEP::perCent);
    fr4->AddElement(elO,30.7*CLHEP::perCent);
    fr4->AddElement(elSi,21.9*CLHEP::perCent);
    fr4->AddElement(elB,2.2*CLHEP::perCent);
    fr4->AddElement(elNa,2.2*CLHEP::perCent);
    geoMan->SetDrawAtt(fr4,kGreen+1,1.0);

    // Set up fr4 material properties
    G4MaterialPropertiesTable *fr4MPT = new G4MaterialPropertiesTable();
    fr4->SetMaterialPropertiesTable(fr4MPT);

    // Set up fr4 for optical photons. (these are bogus values for
    // testing.  Chosen to test optical processses)
    std::vector<G4double> fr4RIndex{
        1.55, 1.55, // 750, 700
        1.55, 1.55, // 650, 600
        1.55, 1.55, // 550, 500
        1.55, 1.55, // 450, 400
        1.55, 1.55, // 350, 300
        1.55, 1.55, // 250, 200
    };
    fr4MPT->AddProperty("RINDEX",photEnergy,fr4RIndex);
    std::vector<G4double> fr4AbsLength{
        300*um, 300*um, // 750, 700
        300*um, 300*um, // 650, 600
        300*um, 300*um, // 550, 500
        300*um, 300*um, // 450, 400
        300*um, 300*um, // 350, 300
        300*um, 300*um, // 250, 200
    };
    fr4MPT->AddProperty("ABSLENGTH",photEnergy,fr4AbsLength);

    // FR4_Copper - Approximated by the composition of G10 plus copper.  The
    // copper is from the cladding, but is approximated as spread through the
    // FR4.  The density is from Wikipedia.
    G4Material* fr4Copper
        = new G4Material(name="FR4_Copper",
                         density = 1850*CLHEP::kg/CLHEP::m3,
                         nel=7);
    double cuFrac = 3*CLHEP::perCent;
    double fr4Frac = 1.0 - cuFrac;
    fr4Copper->AddElement(elH,6.2*CLHEP::perCent*fr4Frac);
    fr4Copper->AddElement(elC,36.8*CLHEP::perCent*fr4Frac);
    fr4Copper->AddElement(elO,30.7*CLHEP::perCent*fr4Frac);
    fr4Copper->AddElement(elSi,21.9*CLHEP::perCent*fr4Frac);
    fr4Copper->AddElement(elB,2.2*CLHEP::perCent*fr4Frac);
    fr4Copper->AddElement(elNa,2.2*CLHEP::perCent*fr4Frac);
    fr4Copper->AddElement(elCu,cuFrac);
    geoMan->SetDrawAtt(fr4Copper,kYellow-6,1.0);

    // Set up fr4Copper material properties
    G4MaterialPropertiesTable *fr4CopperMPT = new G4MaterialPropertiesTable();
    fr4Copper->SetMaterialPropertiesTable(fr4CopperMPT);

    // Set up fr4Copper for optical photons (these are bogus values for
    // testing.  Chosen to test optical processses)
    std::vector<G4double> fr4CopperRIndex{
        1.05, 1.05, // 750, 700
        1.05, 1.05, // 650, 600
        1.05, 1.05, // 550, 500
        1.05, 1.05, // 450, 400
        1.05, 1.05, // 350, 300
        1.05, 1.05, // 250, 200
    };
    fr4CopperMPT->AddProperty("RINDEX",photEnergy,fr4CopperRIndex);
    std::vector<G4double> fr4CopperAbsLength{
        300*um, 300*um, // 750, 700
        300*um, 300*um, // 650, 600
        300*um, 300*um, // 550, 500
        300*um, 300*um, // 450, 400
        300*um, 300*um, // 350, 300
        300*um, 300*um, // 250, 200
    };
    fr4CopperMPT->AddProperty("ABSLENGTH",photEnergy,fr4CopperAbsLength);

    // Acrylic - Approximated by the composition of the Acrylic used to hold
    // the TPB..  The density is from Wikipedia.
    G4Material* acrylic
        = new G4Material(name="Acrylic",
                         density = 1189*CLHEP::kg/CLHEP::m3,
                         nel=3);
    acrylic->AddElement(elH,53.4*CLHEP::perCent);
    acrylic->AddElement(elO,13.3*CLHEP::perCent);
    acrylic->AddElement(elC,33.3*CLHEP::perCent);
    geoMan->SetDrawAtt(acrylic,kAzure+6,0.75);

    // Set up acrylic material properties
    G4MaterialPropertiesTable *acrylicMPT = new G4MaterialPropertiesTable();
    acrylic->SetMaterialPropertiesTable(acrylicMPT);

    // Set up acrylic for optical photons (these are bogus values for
    // testing.  Chosen to test optical processses)
    std::vector<G4double> acrylicRIndex{
        1.45, 1.45, // 750, 700
        1.45, 1.45, // 650, 600
        1.45, 1.45, // 550, 500
        1.45, 1.45, // 450, 400
        1.45, 1.45, // 350, 300
        1.45, 1.45, // 250, 200
    };
    acrylicMPT->AddProperty("RINDEX",photEnergy,acrylicRIndex);
    std::vector<G4double> acrylicAbsLength{
        300*cm, 300*cm, // 750, 700
        300*cm, 300*cm, // 650, 600
        300*cm, 300*cm, // 550, 500
        300*cm, 300*cm, // 450, 400
        30*cm, 3*cm, // 350, 300
        3*cm, 3*cm, // 250, 200
    };
    acrylicMPT->AddProperty("ABSLENGTH",photEnergy,acrylicAbsLength);
}
