#include "EDepSimUserDetectorConstruction.hh"
#include "EDepSimDetectorMessenger.hh"
#include "EDepSimRootGeometryManager.hh"
#include "EDepSimSDFactory.hh"
#include "EDepSimUniformField.hh"
#include "EDepSimEMFieldSetup.hh"
#include "EDepSimArbEMField.hh"
#include "EDepSimArbElecField.hh"
#include "EDepSimArbMagField.hh"

#define BUILD_CAPTAIN
#ifdef BUILD_CAPTAIN
#include "captain/CaptWorldBuilder.hh"
#endif

#include "EDepSimLog.hh"

#include <G4ios.hh>
#include <G4NistManager.hh>
#include <G4StableIsotopes.hh>

#include <G4Isotope.hh>
#include <G4Material.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4RunManager.hh>
#include <G4VPersistencyManager.hh>
#include <G4GDMLParser.hh>
#include <G4UserLimits.hh>

#include <G4SolidStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4PhysicalVolumeStore.hh>

#include <G4FieldManager.hh>

#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

#include <G4RegionStore.hh>

#include <queue>

EDepSim::UserDetectorConstruction::UserDetectorConstruction() {
    fDetectorMessenger = new EDepSim::DetectorMessenger(this);
    fWorldBuilder = NULL;
#define BUILD_CAPTAIN
#ifdef BUILD_CAPTAIN
    fWorldBuilder = new CaptWorldBuilder("/Captain",this);
#endif
    fDefaultMaterial = NULL;
    fValidateGeometry = false;
    fGDMLParser = NULL;
    fPhysicalWorld = NULL;
    new G4UnitDefinition("volt/cm","V/cm","Electric field",volt/cm);
}

EDepSim::UserDetectorConstruction::~UserDetectorConstruction() {
    if (fDetectorMessenger) delete fDetectorMessenger;
    if (fWorldBuilder) delete fWorldBuilder;
    if (fGDMLParser) delete fGDMLParser;
}

namespace {
    double ParseFloat(std::string value) {
        std::istringstream theStream(value);
        double val;
        theStream >> val;
        return val;
    }

    // Parse a string containing an RGBA color.
    G4Color ParseColor(std::string value) {

        std::size_t pos = value.find("(");
        if (pos == std::string::npos) return G4Color(0.5,0.5,0.5,1.0);

        value = value.substr(pos+1);
        pos = value.find(",");
        if (pos == std::string::npos) return G4Color(0.5,0.5,0.5,1.0);
        std::string elem = value.substr(0,pos);
        double r = ParseFloat(elem);

        value = value.substr(pos+1);
        pos = value.find(",");
        if (pos == std::string::npos) return G4Color(0.5,0.5,0.5,1.0);
        elem = value.substr(0,pos);
        double g = ParseFloat(elem);

        value = value.substr(pos+1);
        pos = value.find(",");
        if (pos == std::string::npos) {
            double b = ParseFloat(value);
            return G4Color(r,g,b,1.0);
        }
        elem = value.substr(0,pos);
        double b = ParseFloat(elem);

        value = value.substr(pos+1);
        double a = ParseFloat(value);

        return G4Color(r,g,b,a);
    }
}


namespace {
    double ParseUnit(std::string value, std::string unit) {
        double val = 0.0;
        std::istringstream theStream(value);
        theStream >> val >> unit;
        val *= G4UnitDefinition::GetValueOf(unit);
        return val;
    }

    // Parse a string containing the electric field vector.
    G4ThreeVector ParseEField(std::string value) {
        G4ThreeVector field(0,0,0);
        value = value.substr(value.find("(")+1);
        std::string elem = value.substr(0,value.find(","));
        field.setX(ParseUnit(elem,"V/cm"));
        value = value.substr(value.find(",")+1);
        elem = value.substr(0,value.find(","));
        field.setY(ParseUnit(elem,"V/cm"));
        value = value.substr(value.find(",")+1);
        elem = value.substr(0,value.find(")"));
        field.setZ(ParseUnit(elem,"V/cm"));
        return field;
    }

    G4ThreeVector ParseBField(std::string value) {
        G4ThreeVector field(0,0,0);
        value = value.substr(value.find("(")+1);
        std::string elem = value.substr(0,value.find(","));
        field.setX(ParseUnit(elem,"tesla"));
        value = value.substr(value.find(",")+1);
        elem = value.substr(0,value.find(","));
        field.setY(ParseUnit(elem,"tesla"));
        value = value.substr(value.find(",")+1);
        elem = value.substr(0,value.find(")"));
        field.setZ(ParseUnit(elem,"tesla"));
        return field;
    }
}

G4VPhysicalVolume* EDepSim::UserDetectorConstruction::Construct() {
    if (fGDMLParser) {
        EDepSimLog("Using a GDML Geometry");

        fPhysicalWorld =  fGDMLParser->GetWorldVolume();

        // Use auxilliary information to set the visual attributes for logical
        // volumes.
        for (G4GDMLAuxMapType::const_iterator
                 aux = fGDMLParser->GetAuxMap()->begin();
             aux != fGDMLParser->GetAuxMap()->end();
             ++aux) {
            G4Color color(0.5,0.5,0.5,0.5);
            for (G4GDMLAuxListType::const_iterator auxItem
                     = aux->second.begin();
                 auxItem != aux->second.end();
                 ++auxItem) {
                if (auxItem->type == "Color") {
                    // Set the color while keeping the original alpha.
                    EDepSimInfo("Set volume " << aux->first->GetName()
                               << " color to " << auxItem->value);
                    G4Color tmp;
                    if (G4Color::GetColour(auxItem->value,tmp)) {
                        color = G4Color(tmp.GetRed(),
                                        tmp.GetGreen(),
                                        tmp.GetBlue(),
                                        color.GetAlpha());
                    }
                    else {
                        color = ParseColor(auxItem->value);
                    }
                }
                if (auxItem->type == "Opacity") {
                    // Set the alpha while keeping the original color.
                    EDepSimInfo("Set volume " << aux->first->GetName()
                               << " opacity to " << auxItem->value);
                    double opacity = ParseFloat(auxItem->value);
                    color = G4Color(color.GetRed(),
                                    color.GetGreen(),
                                    color.GetBlue(),
                                    opacity);

                }
            }
            aux->first->SetVisAttributes(new G4VisAttributes(color));
        }

        // Check for step limits
        for (G4GDMLAuxMapType::const_iterator
                 aux = fGDMLParser->GetAuxMap()->begin();
             aux != fGDMLParser->GetAuxMap()->end();
             ++aux) {
            for (G4GDMLAuxListType::const_iterator auxItem
                     = aux->second.begin();
                 auxItem != aux->second.end();
                 ++auxItem) {
                if (auxItem->type == "StepLimit") {
                    double stepLimit = ParseUnit(auxItem->value,"mm");
                    EDepSimLog("Set volume " << aux->first->GetName()
                               << " step limit to " << stepLimit
                               << " from " << auxItem->value);
                    aux->first->SetUserLimits(new G4UserLimits(stepLimit));
                    break;
                }
            }
        }
    }

    if (!fPhysicalWorld) {
        EDepSimLog("Try to construct a custom geometry");
        fPhysicalWorld = ConstructDetector();
    }

    if (!fPhysicalWorld) {
        EDepSimError("Physical world not built");
        EDepSimThrow("Physical world not built");
    }

    EDepSim::RootGeometryManager::Get()->Update(fPhysicalWorld,
                                                fValidateGeometry);

    G4RunManager::GetRunManager()->DefineWorldVolume(fPhysicalWorld);

    G4VPersistencyManager *pMan
        = G4VPersistencyManager::GetPersistencyManager();
    if (pMan) pMan->Store(fPhysicalWorld);

    return fPhysicalWorld;
}

void EDepSim::UserDetectorConstruction::ConstructSDandField() {
    // The parser didn't get created.
    if (!fGDMLParser) return;
    // There isn't an auxillary map associated with the parser.
    if (!fGDMLParser->GetAuxMap()) return;

    // Construct the sensitive detectors for the logical volumes.  This can be
    // done in any order so leave it outside of the traversal below.
    for (G4GDMLAuxMapType::const_iterator
             aux = fGDMLParser->GetAuxMap()->begin();
         aux != fGDMLParser->GetAuxMap()->end();
         ++aux) {
        for (G4GDMLAuxListType::const_iterator auxItem = aux->second.begin();
             auxItem != aux->second.end();
             ++auxItem) {
            if (auxItem->type != "SensDet") continue;
            std::string logName(aux->first->GetName());
            bool exclude = false;
            for (std::vector<std::string>::iterator e
                     = fExcludeAsSensitiveDetector.begin();
                 e != fExcludeAsSensitiveDetector.end(); ++e) {
                if (logName.find((*e)) != std::string::npos) {
                    exclude = true;
                    break;
                }
            }
            if (exclude) {
                EDepSimLog("Volume " << logName << "marked as sensitive, but"
                           << " excluded");
            }
            EDepSimLog("Collect energy deposition for " << aux->first->GetName()
                       << " in " << auxItem->value);
            EDepSim::SDFactory factory("segment");
            aux->first->SetSensitiveDetector(factory.MakeSD(auxItem->value));
        }
    }

    // Add the EM field using a breadth first traversal of the geometry.  This
    // means that the field from parent volumes are applied to children, but
    // that the children can override the local field.
    std::queue<G4LogicalVolume*> remainingVolumes;
    remainingVolumes.push(fPhysicalWorld->GetLogicalVolume());
    while (!remainingVolumes.empty()) {
        // Get the next logical volume and put it's daughters onto queue to be
        // handled later.
        G4LogicalVolume* logVolume = remainingVolumes.front();
        remainingVolumes.pop();
        for (std::size_t i = 0; i<logVolume->GetNoDaughters(); ++i) {
            G4VPhysicalVolume* daughter = logVolume->GetDaughter(i);
            remainingVolumes.push(daughter->GetLogicalVolume());
        }

        // Check to see if there are any auxillary values for this volume.  If
        // not, then we are done.
        G4GDMLAuxMapType::const_iterator aux
            = fGDMLParser->GetAuxMap()->find(logVolume);
        if (aux == fGDMLParser->GetAuxMap()->end()) continue;


        // Check the auxilliary items and add the field if necessary.
        const G4GDMLAuxListType& auxItems = aux->second;

        // Find the electric field for the volume.
        bool HasEField = false;
        std::string eField_fname;
        G4ThreeVector eField(0,0,0);
        for (G4GDMLAuxListType::const_iterator auxItem = auxItems.begin();
             auxItem != auxItems.end();
             ++auxItem) {
            if (auxItem->type != "EField" && auxItem->type != "ArbEField") {
                continue;
            }

            if (auxItem->type == "EField") {
                eField = ParseEField(auxItem->value);
                HasEField = true;

                EDepSimInfo("Set the electric field for "
                        << logVolume->GetName()
                        << " to "
                        << " X=" << eField.x()/(volt/cm) << " V/cm"
                        << ", Y=" << eField.y()/(volt/cm) << " V/cm"
                        << ", Z=" << eField.z()/(volt/cm) << " V/cm");
            }

            if (auxItem->type == "ArbEField") {
                eField_fname = auxItem->value;
                HasEField = true;

                EDepSimInfo("Set the electric field for "
                        << logVolume->GetName()
                        << " to " << eField_fname);
            }
        }

        // Find the magnetic field for the volume.
        bool HasBField = false;
        std::string bField_fname;
        G4ThreeVector bField(0,0,0);
        for (G4GDMLAuxListType::const_iterator auxItem = auxItems.begin();
             auxItem != auxItems.end();
             ++auxItem) {
            if (auxItem->type != "BField" && auxItem->type != "ArbBField") {
                continue;
            }

            if (auxItem->type == "BField") {
                bField = ParseBField(auxItem->value);
                HasBField = true;

                EDepSimInfo("Set the magnetic field for "
                        << logVolume->GetName()
                        << " to "
                        << " X=" << bField.x()/(tesla) << " T"
                        << ", Y=" << bField.y()/(tesla) << " T"
                        << ", Z=" << bField.z()/(tesla) << " T");
            }

            if (auxItem->type == "ArbBField") {
                bField_fname = auxItem->value;
                HasBField = true;

                EDepSimInfo("Set the magnetic field for "
                        << logVolume->GetName()
                        << " to " << bField_fname);
            }
        }

        // Set the electromagnetic field. But first check if there is one to
        // set.
        if (!HasEField && !HasBField) continue;

        // The electric field can't be exactly zero, or the equation of
        // motion fails.
        if (eField.mag() < 0.01 * volt/cm) {
            eField.setY(0.01 * volt/cm);
        }

        // Create new field manager and an arbitrary EM field. The ArbEMField
        // can store fields that inherits from G4ElectroMagneticField.
        G4FieldManager* manager = new G4FieldManager();
        EDepSim::ArbEMField* arbField = new EDepSim::ArbEMField();

        if (!eField_fname.empty()) {
            EDepSim::ArbElecField* eFieldPtr = new EDepSim::ArbElecField();
            eFieldPtr->ReadFile(eField_fname);
            eFieldPtr->PrintInfo();

            arbField->SetEField(eFieldPtr);
        }
        else {
            EDepSim::UniformField* eFieldPtr = new EDepSim::UniformField();
            eFieldPtr->SetEField(eField);

            arbField->SetEField(eFieldPtr);
        }

        if (!bField_fname.empty()) {
            EDepSim::ArbMagField* bFieldPtr = new EDepSim::ArbMagField();
            bFieldPtr->ReadFile(bField_fname);
            bFieldPtr->PrintInfo();

            arbField->SetBField(bFieldPtr);
        }
        else {
            EDepSim::UniformField* bFieldPtr = new EDepSim::UniformField();
            bFieldPtr->SetBField(bField);

            arbField->SetBField(bFieldPtr);
        }

        EDepSim::EMFieldSetup* fieldSetup
                         = new EDepSim::EMFieldSetup(arbField, manager);
        if (!fieldSetup) {
            EDepSimError("Field not created");
            throw std::runtime_error("Field not created");
        }
        logVolume->SetFieldManager(manager,true);
    }
}

void EDepSim::UserDetectorConstruction::DefineMaterials() {
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

    //Air
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

    // This is the default material.
    fDefaultMaterial = air;

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

    // Liquid Argon
    G4Material* LAr =  new G4Material(name="Argon_Liquid",
                                      density = 1.3954*CLHEP::g/CLHEP::cm3,
                                      nel=1,
                                      kStateLiquid,
                                      temperature = 87.3*CLHEP::kelvin,
                                      pressure=1*CLHEP::atmosphere);
    LAr->AddElement(elAr, natoms=1);

    // Set up liquid argon for NEST.
    G4MaterialPropertiesTable *LArMatProps = new G4MaterialPropertiesTable();
    LArMatProps->AddConstProperty("ELECTRICFIELD",500*CLHEP::volt/CLHEP::cm);
    LArMatProps->AddConstProperty("TOTALNUM_INT_SITES",-1);
    LAr->SetMaterialPropertiesTable(LArMatProps);

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

    // Print all the materials defined.
    EDepSimLog(*(G4Material::GetMaterialTable()));
}

G4Element* EDepSim::UserDetectorConstruction::DefineElement(G4String name,
                                                        G4String symbol,
                                                        G4double z) {
    // Keep track of existing isotopes.  This prevents them from being created
    // twice when the geometry is regenerated.
    static G4StableIsotopes theDefaultIsotopes;

    G4int Z = G4int(z);
    G4int ncomponents = theDefaultIsotopes.GetNumberOfIsotopes(Z);
    G4Element* el = new G4Element(name, symbol, ncomponents);
    G4int first = theDefaultIsotopes.GetFirstIsotope(Z);
    for (G4int i=0; i<ncomponents; i++) {
        G4int A = theDefaultIsotopes.GetIsotopeNucleonCount(first+i);
        std::ostringstream os; os << symbol << A;
        G4Isotope* is = new G4Isotope(name=os.str(), Z, A,
                                      A*CLHEP::g/CLHEP::mole);
        G4double abundance = theDefaultIsotopes.GetAbundance(first+i);
        el->AddIsotope(is, abundance*CLHEP::perCent);
    }
    return el;
}

G4VPhysicalVolume* EDepSim::UserDetectorConstruction::ConstructDetector() {

    if (!fWorldBuilder) return NULL;

    DefineMaterials();

    // Create a region outside of the detector to define cuts.
    G4RegionStore::GetInstance()->FindOrCreateRegion("hallRegion");

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

    return physWorld;
}

void EDepSim::UserDetectorConstruction::UpdateGeometry() {
    // Initialize the run manager.  This sets everything in motion.
    EDepSimLog("Update the geometry and initialize the G4RunManager");
    G4RunManager::GetRunManager()->Initialize();
}
