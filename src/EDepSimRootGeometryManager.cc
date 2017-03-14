////////////////////////////////////////////////////////////
//
#include "EDepSimRootGeometryManager.hh"
#include "EDepSimException.hh"

#include "EDepSimLog.hh"

#include <TROOT.h>
#include <TMath.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoMedium.h>
#include <TGeoElement.h>
#include <TGeoTube.h>
#include <TGeoTrd2.h>
#include <TGeoSphere.h>
#include <TGeoPgon.h>
#include <TGeoArb8.h>
#include <TGeoBoolNode.h>
#include <TGeoCompositeShape.h>
#include <TGeoMatrix.h>
#include <TGeoOverlap.h>
#include <TGeoXtru.h>
#include <TGeoPcon.h>
#include <TColor.h>

#include <globals.hh>

#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Material.hh>
#include <G4Element.hh>
#include <G4Isotope.hh>
#include <G4UnitsTable.hh>

#include <G4VisAttributes.hh>
#include <G4VSolid.hh>
#include <G4Box.hh>
#include <G4Trd.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4Polyhedra.hh>
#include <G4Polycone.hh>
#include <G4Trap.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4ExtrudedSolid.hh>

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>


#include <memory>
#include <cmath>
#include <cstdlib>
#include <set>

EDepSim::RootGeometryManager* EDepSim::RootGeometryManager::fThis = NULL;

EDepSim::RootGeometryManager::RootGeometryManager() {
}

EDepSim::RootGeometryManager* EDepSim::RootGeometryManager::Get() {
    if (!fThis) fThis = new EDepSim::RootGeometryManager();
    return fThis;
}

EDepSim::RootGeometryManager::~RootGeometryManager() {}

void EDepSim::RootGeometryManager::Export(const char *file) {
    EDepSimLog("   *** Export to " << file);
    gGeoManager->Export(file);
}

int EDepSim::RootGeometryManager::GetNodeId(const G4ThreeVector& pos) {
    if (!gGeoManager) return -1;
    gGeoManager->FindNode(pos.x(), pos.y(), pos.z());
    return gGeoManager->GetCurrentNodeId();
}

void EDepSim::RootGeometryManager::Update(const G4VPhysicalVolume* aWorld,
                                      bool validateGeometry) {
    EDepSimLog("%%%%%%%%%%%%%%%%%%%%%%%%%% Update ROOT Geometry "
             << "%%%%%%%%%%%%%%%%%%%%%%%%%%" );
    if (gGeoManager) {
        EDepSimLog("%%%%%%%%%%%%%%% Warning: Replacing ROOT Geometry ");
        delete gGeoManager;
        // Clear the node counts definitions.
        fNodeCount.clear();
        // Clear the cached materials.  The material objects are owned by the
        // TGeoManager and are invalid after it has been deleted.
        fMedium.clear();
        // Clear the cached isotopes.  This is a bit complicated since the
        // isotope pointers may be duplicated in the map.
        std::set<TGeoElement*> isotope;
        for (std::map<G4String,TGeoElement*>::iterator i = fIsotope.begin();
             i != fIsotope.end();
             ++i) {
            isotope.insert(i->second);
        }
        fIsotope.clear();
        for (std::set<TGeoElement*>::iterator i = isotope.begin();
             i != isotope.end();
             ++i) {
            delete *i;
        }
    }
    // Create the new geometry.
    gGeoManager = new TGeoManager("EDepSimGeometry",
                                  "Simulated Detector Geometry");
    // Create all of the materials.
    CreateMaterials(aWorld);
    // Create the ROOT geometry definitions.
    fPrintedMass.clear();
    fNameStack.clear();
    CreateEnvelope(aWorld,gGeoManager,NULL);
    gGeoManager->CloseGeometry("di");

    EDepSimLog("Geometry initialized and closed");
    
    if (!validateGeometry) return;

    // Check for overlaps at volume corners.  Look for overlaps at 0.1 mm size.
    gGeoManager->CheckOverlaps(0.1*CLHEP::mm);
    {
        TIter next(gGeoManager->GetListOfOverlaps());
        int count = 0;
        TGeoOverlap* overlap;
        while ((overlap=(TGeoOverlap*)next())) {
            ++count;
            overlap->PrintInfo();
        }
        if (count > 0) {
            EDepSimThrow("Geometry has overlaps");
        }
    }

    // Check for overlaps with sampling.  Look for overlaps at 0.1 mm size.
    gGeoManager->CheckOverlaps(0.1*CLHEP::mm,"s100000");
    {
        TIter next(gGeoManager->GetListOfOverlaps());
        int count = 0;
        TGeoOverlap* overlap;
        while ((overlap=(TGeoOverlap*)next())) {
            ++count;
            overlap->PrintInfo();
        }
        if (count > 0) {
            EDepSimThrow("Geometry has Overlaps");
        }
    }

    EDepSimLog("Geometry validated");
}

TGeoShape* EDepSim::RootGeometryManager::CreateShape(const G4VSolid* theSolid, 
                                                 TGeoMatrix **returnMatrix) {
    const G4String geometryType = theSolid->GetEntityType();
    TGeoShape* theShape = NULL;
    if (geometryType == "G4Box") {
        // Create a box
        const G4Box* box = dynamic_cast<const G4Box*>(theSolid);
        theShape = new TGeoBBox(box->GetXHalfLength()/CLHEP::mm,
                                box->GetYHalfLength()/CLHEP::mm,
                                box->GetZHalfLength()/CLHEP::mm);
    }
    else if (geometryType == "G4Tubs") {
        const G4Tubs* tube = dynamic_cast<const G4Tubs*>(theSolid);
        // Root takes the angles in degrees so there is no extra
        // conversion.
        double zhalf = tube->GetZHalfLength()/CLHEP::mm;
        double rmin = tube->GetInnerRadius()/CLHEP::mm;
        double rmax = tube->GetOuterRadius()/CLHEP::mm;
        double minPhiDeg = tube->GetStartPhiAngle()/CLHEP::degree;
        double maxPhiDeg = minPhiDeg + tube->GetDeltaPhiAngle()/CLHEP::degree;
        theShape = new TGeoTubeSeg(rmin, rmax,
                                   zhalf,
                                   minPhiDeg, maxPhiDeg);
    }
    else if (geometryType == "G4Sphere") {
        const G4Sphere* sphere = dynamic_cast<const G4Sphere*>(theSolid);
        // Root takes the angles in degrees so there is no extra
        // conversion.
        double minPhiDeg = sphere->GetStartPhiAngle()/CLHEP::degree;
        double maxPhiDeg = minPhiDeg + sphere->GetDeltaPhiAngle()/CLHEP::degree;
        double minThetaDeg = sphere->GetStartThetaAngle()/CLHEP::degree;
        double maxThetaDeg = minThetaDeg + sphere->GetDeltaThetaAngle()/CLHEP::degree;
        theShape = new TGeoSphere(sphere->GetInsideRadius()/CLHEP::mm,
                                  sphere->GetOuterRadius()/CLHEP::mm,
                                  minThetaDeg, maxThetaDeg,
                                  minPhiDeg, maxPhiDeg);
    }
    else if (geometryType == "G4Polyhedra") {
        const G4Polyhedra* polyhedra 
            = dynamic_cast<const G4Polyhedra*>(theSolid);
        double phi = polyhedra->GetStartPhi();
        double dPhi = polyhedra->GetEndPhi() - phi;
        if (dPhi>2*M_PI) dPhi -= 2*M_PI;
        if (dPhi<0) dPhi += 2*M_PI;
        int sides = polyhedra->GetNumSide();
        int numZ = polyhedra->GetNumRZCorner()/2;
        // Factor to take into account that ROOT uses the circle that can be
        // inscribed inside the polygon, and G4 uses the corner
        double g4Factor = std::cos(0.5*dPhi/sides);
        TGeoPgon* pgon = new TGeoPgon(phi/CLHEP::degree, dPhi/CLHEP::degree, sides, numZ);
        for (int i = 0; i< numZ; ++i) {
            pgon->DefineSection(i,
                                polyhedra->GetCorner(numZ-i-1).z/CLHEP::mm,
                                g4Factor*polyhedra->GetCorner(numZ-i-1).r/CLHEP::mm,
                                g4Factor*polyhedra->GetCorner(numZ+i).r/CLHEP::mm);
        }
        theShape = pgon;
    }
    else if (geometryType == "G4Polycone") {
        const G4Polycone* polycone
            = dynamic_cast<const G4Polycone*>(theSolid);
        double phi = polycone->GetStartPhi();
        double dPhi = polycone->GetEndPhi() - phi;
        if (dPhi>2*M_PI) dPhi -= 2*M_PI;
        if (dPhi<0) dPhi += 2*M_PI;
#ifdef USE_POLYCONE_CORNERS
        int numZ = polycone->GetNumRZCorner()/2;
        TGeoPcon* pcon = new TGeoPcon(phi/CLHEP::degree, dPhi/CLHEP::degree, numZ);
        // This depends on the (mostly) undocumented order of the corners in
        // the G4Polycone internals.  It's a little unstable...
        for (int i = 0; i< numZ; ++i) {
            pcon->DefineSection(i,
                                polycone->GetCorner(numZ-i-1).z/CLHEP::mm,
                                polycone->GetCorner(numZ-i-1).r/CLHEP::mm,
                                polycone->GetCorner(numZ+i).r/CLHEP::mm);
        }
#else
        const G4PolyconeHistorical* param = polycone->GetOriginalParameters();
        int numZ = param->Num_z_planes;
        TGeoPcon* pcon = new TGeoPcon(phi/CLHEP::degree, dPhi/CLHEP::degree, numZ);
        // This depends on the older interface.  It's not marked as
        // deprecated, but the documentation discourages it's use.
        for (int i = 0; i< numZ; ++i) {
            pcon->DefineSection(i,
                                param->Z_values[i]/CLHEP::mm,
                                param->Rmin[i]/CLHEP::mm,
                                param->Rmax[i]/CLHEP::mm);
        }
#endif
        theShape = pcon;
    }
    else if (geometryType == "G4Trap") {
        const G4Trap* trap
            = dynamic_cast<const G4Trap*>(theSolid);
        double dz = trap->GetZHalfLength()/CLHEP::mm;
        double theta = 0;
        double phi = 0;
        double h1 = trap->GetYHalfLength1()/CLHEP::mm;
        double bl1 = trap->GetXHalfLength1()/CLHEP::mm;
        double tl1 = trap->GetXHalfLength2()/CLHEP::mm;
        double alpha1 = std::atan(trap->GetTanAlpha1())/CLHEP::degree;
        double h2 = trap->GetYHalfLength2()/CLHEP::mm;
        double bl2 = trap->GetXHalfLength3()/CLHEP::mm;
        double tl2 = trap->GetXHalfLength4()/CLHEP::mm;
        double alpha2 = std::atan(trap->GetTanAlpha2())/CLHEP::degree;
        theShape = new TGeoTrap(dz, theta, phi,
                                h1, bl1, tl1, alpha1,
                                h2, bl2, tl2, alpha2);
    }
    else if (geometryType == "G4Trd") {
        const G4Trd* trd
            = dynamic_cast<const G4Trd*>(theSolid);
        double dz = trd->GetZHalfLength()/CLHEP::mm;
        double dx1 = trd->GetXHalfLength1()/CLHEP::mm;
        double dx2 = trd->GetXHalfLength2()/CLHEP::mm;
        double dy1 = trd->GetYHalfLength1()/CLHEP::mm;
        double dy2 = trd->GetYHalfLength2()/CLHEP::mm;
        theShape = new TGeoTrd2(dx1,dx2,dy1,dy2,dz);
    }
    else if (geometryType == "G4SubtractionSolid") {
        const G4SubtractionSolid* sub
            = dynamic_cast<const G4SubtractionSolid*>(theSolid);
        const G4VSolid* solidA = sub->GetConstituentSolid(0);
        const G4VSolid* solidB = sub->GetConstituentSolid(1);
        // solidA - solidB
        TGeoMatrix* matrixA = NULL;
        TGeoShape* shapeA = CreateShape(solidA, &matrixA);
        TGeoMatrix* matrixB = NULL;
        TGeoShape* shapeB = CreateShape(solidB, &matrixB);
        TGeoSubtraction* subtractNode = new TGeoSubtraction(shapeA,shapeB,
                                                            matrixA, matrixB);
        theShape = new TGeoCompositeShape("name",subtractNode);
    }
    else if (geometryType == "G4DisplacedSolid") {
        const G4DisplacedSolid* disp
            = dynamic_cast<const G4DisplacedSolid*>(theSolid);
        const G4VSolid* movedSolid = disp->GetConstituentMovedSolid();
        G4RotationMatrix rotation = disp->GetObjectRotation();
        G4ThreeVector displacement = disp->GetObjectTranslation();
        theShape = CreateShape(movedSolid);
        if (returnMatrix) {
            TGeoRotation* rotate
                = new TGeoRotation("rot",
                                   TMath::RadToDeg()*rotation.thetaX(),
                                   TMath::RadToDeg()*rotation.phiX(),
                                   TMath::RadToDeg()*rotation.thetaY(),
                                   TMath::RadToDeg()*rotation.phiY(),
                                   TMath::RadToDeg()*rotation.thetaZ(),
                                   TMath::RadToDeg()*rotation.phiZ());
            *returnMatrix = new TGeoCombiTrans(displacement.x()/CLHEP::mm,
                                               displacement.y()/CLHEP::mm,
                                               displacement.z()/CLHEP::mm,
                                               rotate);
        }
    }
    else if (geometryType == "G4UnionSolid") {
        const G4UnionSolid* sub
            = dynamic_cast<const G4UnionSolid*>(theSolid);
        const G4VSolid* solidA = sub->GetConstituentSolid(0);
        const G4VSolid* solidB = sub->GetConstituentSolid(1);
        // solidA - solidB
        TGeoMatrix* matrixA = NULL;
        TGeoShape* shapeA = CreateShape(solidA, &matrixA);
        TGeoMatrix* matrixB = NULL;
        TGeoShape* shapeB = CreateShape(solidB, &matrixB);
        TGeoUnion* unionNode = new TGeoUnion(shapeA,  shapeB,
                                             matrixA, matrixB);
        theShape = new TGeoCompositeShape("name",unionNode);
    }
    else if (geometryType == "G4ExtrudedSolid"){
        //This following only works when using the 'standard'
        //G4ExtrudedSolid Constructor.

        const G4ExtrudedSolid* extr
            = dynamic_cast<const G4ExtrudedSolid*>(theSolid);
        
        //number of z planes
        const G4int nZ = extr->GetNofZSections();
        //number of vertices in the polygon
        const G4int nV = extr->GetNofVertices();
        
        //define and pointers
        const int maxVertices = 1000;
        double vertices_x[maxVertices];
        double vertices_y[maxVertices];
        if (maxVertices < nV) {
            EDepSimThrow("Polygon with more than maxVertices");
        }
        
        
        //define an intermediate extrusion constructor with nZ z planes.
        TGeoXtru *xtru = new TGeoXtru(nZ);
        
        //Get the polygons points.
        std::vector<G4TwoVector> polyPoints = extr->GetPolygon();
        
        //fill the vertices arrays
        for(int i = 0 ; i < nV ; i++){
            vertices_x[i]= polyPoints[i].x();
            vertices_y[i]= polyPoints[i].y();
        }
        
        //Define the polygon
        xtru->DefinePolygon(nV, vertices_x, vertices_y);
        
        double z_pos, x_off, y_off, scale;
        
        //fill the parameters to define the Root extruded solid
        for(int i = 0 ; i < nZ ; i++){
            z_pos = extr->GetZSection(i).fZ;
            x_off = extr->GetZSection(i).fOffset.x() ;
            y_off = extr->GetZSection(i).fOffset.y();
            scale = extr->GetZSection(i).fScale;
            xtru->DefineSection(i, z_pos, x_off, y_off, scale);
        }
        //now assign 'theShape' to this complete extruded object.
        theShape = xtru;
    }
    else {
        EDepSimThrow("shape not implemented");
    }
    
    return theShape;
}

TGeoVolume* EDepSim::RootGeometryManager::CreateVolume(TGeoManager* theEnvelope, 
                                                   const G4VSolid* theSolid, 
                                                   std::string theName,
                                                   TGeoMedium* theMedium) {
    TGeoShape* theShape = CreateShape(theSolid);
    TGeoVolume* theVolume = new TGeoVolume(theName.c_str(),
                                           theShape,
                                           theMedium);
    return theVolume;
}
                                                   
// Determine if a volume should copied to the ROOT geometry representation.
// If this returns true, then the volume and all of it's children will not be
// exported.
bool EDepSim::RootGeometryManager::IgnoreVolume(const G4VPhysicalVolume* theVol) {
    std::string theFullName = theVol->GetName();
    std::string theShortName = theFullName;
    theShortName.erase(0,theShortName.rfind("/")+1);

    // Don't save the internal structure of extruded scintillating bars.  This
    // is required to make sure that hits get assigned to the right geometry
    // volume in the ROOT geometry.
    if (theFullName.find("Bar/Core") != std::string::npos) return true;
    if (theFullName.find("Bar/CrnrOfBar") != std::string::npos) return true;
    if (theFullName.find("Bar/SideOfBar") != std::string::npos) return true;

    return false;
}

// Determine if the volume should be printed to the output.
bool EDepSim::RootGeometryManager::PrintMass(const G4VPhysicalVolume* theVol) {
    std::string theFullName = theVol->GetName();

    for (std::vector<G4String>::iterator n = fPrintMass.begin();
         n != fPrintMass.end();
         ++n) {
        if (theFullName == (*n)) {
            if (fPrintedMass.find(*n) != fPrintedMass.end()) continue;
            fPrintedMass.insert(*n);
            return true;
        }
    }

    return false;
}

// Create all of the materials in the detector and make a translation table
// between material and material name, and element and isotope name.
void EDepSim::RootGeometryManager::CreateMaterials(
    const G4VPhysicalVolume* theG4PhysVol) {

    G4LogicalVolume* theLog = theG4PhysVol->GetLogicalVolume();

    // Find the medium for this volume and create it if it doesn't exist.
    TGeoMedium *theMedium = fMedium[theLog->GetMaterial()->GetName()];

    if (!theMedium) {
        G4Material *mat = theLog->GetMaterial();
        if (mat->GetNumberOfElements()==0) {
            EDepSimError("Material without elements " << mat->GetName());
            EDepSimThrow("Material defined without elements");
        }
        // Make a mixture
        TGeoMixture *theMix
            = new TGeoMixture(mat->GetName(),
                              mat->GetNumberOfElements(),
                              mat->GetDensity());
        unsigned ielement = 0;
        theMix->SetTemperature(mat->GetTemperature());
        // The minus sign is to make sure this over-rides the approximate ROOT
        // radiation and interaction length calculations.
        switch (mat->GetState()) {
        case kStateSolid:
            theMix->SetState(TGeoMaterial::kMatStateSolid);
            break;
        case kStateLiquid:
            theMix->SetState(TGeoMaterial::kMatStateLiquid);
            theMix->SetPressure(mat->GetPressure());
            break;
        case kStateGas:
            theMix->SetState(TGeoMaterial::kMatStateGas);
            theMix->SetPressure(mat->GetPressure());
            break;
        default:
            theMix->SetState(TGeoMaterial::kMatStateUndefined);
        }
        for (unsigned i = 0; i < mat->GetNumberOfElements(); ++i) {
            const G4Element *elem = mat->GetElement(i);
            for (unsigned j = 0; j < elem->GetNumberOfIsotopes(); ++j) {
                const G4Isotope *iso = elem->GetIsotope(j);
                theMix->DefineElement(ielement,
                                      iso->GetA()/(CLHEP::g/CLHEP::mole),
                                      iso->GetZ(),
                                      (mat->GetFractionVector()[i])*
                                      (elem->GetRelativeAbundanceVector()[j]));
                // Find the isotope for this element and create it if it
                // doesn't exist
                TGeoElement *theIsotope = fIsotope[iso->GetName()];
                if (!theIsotope) {
                    // Make an element (isotope)
                    TGeoElement *theEle
                        = new TGeoElement(iso->GetName(),
                                          elem->GetSymbol(),
                                          iso->GetZ(),
                                          iso->GetA()/(CLHEP::g/CLHEP::mole));
                    fIsotope[iso->GetName()] = theEle;
                }
                ielement++;
            }
        }
        theMix->SetRadLen(-mat->GetRadlen(), -mat->GetNuclearInterLength());
        int numed = fMedium.size() + 1;
        theMedium = new TGeoMedium(mat->GetName(),numed,theMix);
        fMedium[mat->GetName()] = theMedium;
    }

    // Recurse through the children.
    for (int child = 0;
         child < theLog->GetNoDaughters();
         ++child) {
        G4VPhysicalVolume* theChild = theLog->GetDaughter(child);
        CreateMaterials(theChild);
    }
}

// Method counts how many nodes there are in mother volume with the same name
// as daughter node.  Will be used in setting index for daughter node.
int EDepSim::RootGeometryManager::HowManySimilarNodesInVolume(
    TGeoVolume* theMother, std::string daughterName) {
    daughterName = daughterName + "_";
    int ndaughters = theMother->GetNdaughters();

    int ndaughters_samename = 0;
    for(int i = 0; i < ndaughters; i++){
        TGeoNode *node = theMother->GetNode(i);
        if(node){
            std::string name(node->GetName());
            if(name.find(daughterName.c_str()) != std::string::npos)
                ndaughters_samename++;
        }
    }

    return ndaughters_samename;

}

// Save the detector envelope.  This is called recursively to fill in the
// entire detector.  The G4 physical volume, theVol, is used to create a
// new root TGeoVolume which is added to the existing root TGeoVolume,
// theMother, as a daughter.
bool EDepSim::RootGeometryManager::CreateEnvelope(
    const G4VPhysicalVolume* theG4PhysVol,
    TGeoManager* theEnvelope,
    TGeoVolume* theMother) {
    
    if (IgnoreVolume(theG4PhysVol)) return true;

    // The new volume that will be added to the mother volume.  This is
    // created in this function.
    TGeoVolume* theVolume = NULL; 
    G4LogicalVolume* theLog = theG4PhysVol->GetLogicalVolume();

    if (PrintMass(theG4PhysVol)) {
        EDepSimLog("%%% Mass: " << theLog->GetMass(true)/CLHEP::kg/1000.0
                   << " ton"
                   << " Volume: " << theG4PhysVol->GetName());
    }
    
    // Get the name of the expected name of the volume.
    std::string theVolumeName;
    for (std::vector<G4String>::iterator n = fNameStack.begin();
         n != fNameStack.end();
         ++n) {
        theVolumeName += "/";
        theVolumeName += *n;
    }

    // Get the volume information from G4.
    const G4VSolid* theSolid = theLog->GetSolid();
    const G4String geometryType = theSolid->GetEntityType();
    std::string theFullName = theG4PhysVol->GetName();
    std::string theShortName = theFullName;
    theShortName.erase(0,theShortName.rfind("/")+1);

    // The volume name is built from all the short names in the path to this
    // volume.  This is different than the full name which is built based on
    // the constructor tree.
    theVolumeName += "/";
    theVolumeName += theShortName;

    // Check the volume names for validity.  If they don't match then just
    // force it.
    if (theShortName == theFullName) {
        theFullName = theVolumeName;
    }

    // Make sure there isn't a bug in the naming.  There should never be a
    // "//" in the name.
    if (theFullName.find("//") != std::string::npos) {
        EDepSimError("Invalid volume name: " << theFullName);
        EDepSimError("   Expected name is: " << theVolumeName);
        theFullName = theVolumeName;
    }

    // Find the medium for this volume.
    std::string materialName = theLog->GetMaterial()->GetName();
    TGeoMedium *theMedium = fMedium[materialName];
    if (!theMedium) {
        EDepSimError("MISSING MATERIAL IS " << materialName);
        EDepSimThrow("Material definition is missing");
    }

    // Create the root volume (the solid in G4 lingo).
    theVolume = CreateVolume(theEnvelope, theSolid, theShortName, theMedium);
    if (!theVolume) theVolume = theMother;
    theVolume->SetTitle(theVolumeName.c_str());
    int color = GetColor(theG4PhysVol);
    theVolume->SetLineColor(color);
    if (GetFill(theG4PhysVol)>0) {
        theVolume->SetFillColor(color);
        theVolume->SetFillStyle(GetFill(theG4PhysVol));
    }

    // There is no mother so set this as the top volume.
    if (!theMother) {
        EDepSimLog("Making \"" << theVolume->GetName() << "\" the top\n");
        theEnvelope->SetTopVolume(theVolume);
    }

    // Push the name of this volume onto the stack before creating the
    // children.
    fNameStack.push_back(theShortName);

    // Add the children to the daughter.
    double missingMass = 0.0;
    for (int child = 0;
         child < theLog->GetNoDaughters();
         ++child) {
        G4VPhysicalVolume* theChild = theLog->GetDaughter(child);
        if (CreateEnvelope(theChild, theEnvelope, theVolume)) {
            G4LogicalVolume *skippedVolume = theChild->GetLogicalVolume();
            missingMass += skippedVolume->GetMass(true);
        }
    }

    // Remove this volume from the name stack.
    fNameStack.pop_back();

    // A some daughter volumes were ignored and so the density of this volume
    // needs to be adjusted.
    if (missingMass > 0.0) {
        // The correction of the material needs to be implemented.
        double totalMass = theLog->GetMass(true);
        double totalVolume = theLog->GetSolid()->GetCubicVolume();
        double totalDensity = totalMass/totalVolume;
        EDepSimNamedDebug("ROOTGeom", "Skipping sub-volumes. Correct "
                        << theMedium->GetName() << " density "
                        << theMedium->GetMaterial()->GetDensity()/(CLHEP::g/CLHEP::cm3)
                        << " g/cm3 to " << totalDensity/(CLHEP::g/CLHEP::cm3) << " g/cm3");
        TGeoMedium *avgMedium = AverageMaterial(theG4PhysVol);
        if (avgMedium) theVolume->SetMedium(avgMedium);
    }

    // Apply the rotation for theMother volume.  This is only done if the
    // theMother is not the top level volume for the envelope so that the
    // detector has the proper orientation. 
    if (theMother && (theMother != theVolume)) {
        G4ThreeVector trans = theG4PhysVol->GetObjectTranslation();
        G4RotationMatrix* rot = theG4PhysVol->GetObjectRotation();
        TGeoRotation* rotate = 
            new TGeoRotation("rot", 
                             TMath::RadToDeg()*rot->thetaX(),
                             TMath::RadToDeg()*rot->phiX(),
                             TMath::RadToDeg()*rot->thetaY(),
                             TMath::RadToDeg()*rot->phiY(),
                             TMath::RadToDeg()*rot->thetaZ(),
                             TMath::RadToDeg()*rot->phiZ());
        if (theG4PhysVol->IsReplicated()) {
            EAxis a; G4int nRep; G4double w; G4double o; G4bool c;
            G4ThreeVector axis;
            theG4PhysVol->GetReplicationData(a,nRep,w,o,c);
            switch (a) {
            case kXAxis: axis.set(1,0,0); break;
            case kYAxis: axis.set(0,1,0); break;
            case kZAxis: axis.set(0,0,1); break;
            default: 
                EDepSimThrow("EDepSim::RootGeometryManager::CreateEnvelope:"
                            "Bad replication data.");
            }
            for (int i=0; i<nRep; ++i) {
                G4ThreeVector pos = axis*w*(i-0.5*(nRep-1));
                TGeoCombiTrans* combi 
                    = new TGeoCombiTrans(pos.x()/CLHEP::mm,
                                         pos.y()/CLHEP::mm,
                                         pos.z()/CLHEP::mm,
                                         rotate);
                int index = HowManySimilarNodesInVolume(theMother, 
                                                        theVolume->GetName());
                theMother->AddNode(theVolume,index,combi);
            }
        }
        else {
            TGeoCombiTrans* combi 
                = new TGeoCombiTrans(trans.x()/CLHEP::mm,
                                     trans.y()/CLHEP::mm,
                                     trans.z()/CLHEP::mm,
                                     rotate);
            int index = HowManySimilarNodesInVolume(theMother, 
                                                    theVolume->GetName());
            theMother->AddNode(theVolume,index,combi);
        }
    }

    return false;
}

void EDepSim::RootGeometryManager::SetDrawAtt(G4Material* material,
                                              int color, double opacity) {
    G4String materialName = material->GetName();    
    fColorMap[materialName].color = color;
    fColorMap[materialName].alpha = opacity;
    if (opacity<0.01) fColorMap[materialName].fill = 0;
    else if (opacity>0.99) fColorMap[materialName].fill = 1;
    else {
        fColorMap[materialName].fill = 4000+100*opacity;
    }
}
                                          
int EDepSim::RootGeometryManager::GetFill(const G4VPhysicalVolume* vol) {
#ifdef USE_FILL
    const G4LogicalVolume* log = vol->GetLogicalVolume();
    G4String materialName = log->GetMaterial()->GetName();
    std::string theFullName = vol->GetName();

    AttributeMap::const_iterator colorPair = fColorMap.find(materialName);

    if (colorPair == fColorMap.end()) {
        EDepSimError("Missing color for \"" << materialName << "\""
                   " in volume " << theFullName);
        fColorMap[materialName].color = kOrange+1;
        fColorMap[materialName].fill = 0;
        return 0;
    }

    return colorPair->second.fill;
#else
    return -1;
#endif
}

G4Color EDepSim::RootGeometryManager::GetG4Color(G4Material* material) {
    G4String materialName = material->GetName();
    AttributeMap::const_iterator colorPair = fColorMap.find(materialName);

    int index = 0;
    double alpha = 1.0;

    if (colorPair == fColorMap.end()) {
        EDepSimError("Missing color for \"" << materialName << "\"");
        fColorMap[materialName].color = kOrange+1;
        fColorMap[materialName].fill = 0;
        index = kOrange+1;
    }
    else {
        index = colorPair->second.color;
        alpha = colorPair->second.alpha;
    }

#include "rootColorToG4ColorMap.hxx"

    G4Color color = sRootColorToG4ColorMap[index];

    return G4Color(color.GetRed(),color.GetGreen(),color.GetBlue(),
                   color.GetAlpha()*alpha);
}

int EDepSim::RootGeometryManager::GetColor(const G4VPhysicalVolume* vol) {
    const G4LogicalVolume* log = vol->GetLogicalVolume();
    G4String materialName = log->GetMaterial()->GetName();
    std::string theFullName = vol->GetName();

    AttributeMap::const_iterator colorPair = fColorMap.find(materialName);
    
    if (colorPair == fColorMap.end()) {
        EDepSimError("Missing color for \"" << materialName << "\""
                   " in volume " << theFullName);
        fColorMap[materialName].color = kOrange+1;
        fColorMap[materialName].fill = 0;
        return kOrange+1;
    }
    
    return colorPair->second.color;
}

std::string EDepSim::RootGeometryManager::MaterialName(
    const G4VPhysicalVolume* thePhys) {
    std::string theFullName = thePhys->GetName();
    G4LogicalVolume* theLog = thePhys->GetLogicalVolume();
    std::string materialName = theLog->GetMaterial()->GetName();

    // If this is a scintillator bar, then the base material name should be
    // either FGDScintillator or Scintillator.
    if (theFullName.find("/Bar") != std::string::npos) {
        if (theFullName.find("/FGD") != std::string::npos) {
            materialName = "FGDScintillator";
        }
        else if (theFullName.find("/P0D/") != std::string::npos) {
            materialName = "P0DScintillator";
        }
        else {
            materialName = "Scintillator";
        }
    }

    return materialName;
}

TGeoMedium* EDepSim::RootGeometryManager::AverageMaterial(
    const G4VPhysicalVolume* thePhys) {
    G4LogicalVolume* theLog = thePhys->GetLogicalVolume();
    double totalMass = theLog->GetMass(true);
    double totalVolume = theLog->GetSolid()->GetCubicVolume();
    double totalDensity = totalMass/totalVolume;
    std::ostringstream nameStream;
    nameStream << MaterialName(thePhys) 
               << "Avg" << totalDensity/(CLHEP::g/CLHEP::cm3);
    std::string averageName = nameStream.str();
    TGeoMedium* avgMedium = fMedium[averageName];
    if (avgMedium) return avgMedium;
    // Create a map of the material pointer and the mass of that material that
    // is contributing to the mixture.  This will be used to calculate the
    // average radiation length and interaction length.
    std::map< const G4Material* , double > materialMass;
    // Create a map of element names and the amount of that element in the
    // mixture.
    std::map<std::string,double> componentMass;
    // Create a stack to walk the geometry tree.
    std::vector<const G4VPhysicalVolume*> stack;
    stack.push_back(thePhys);
    while (!stack.empty()) {
        // Get the new current volume off the stack.
        const G4VPhysicalVolume* currentPhys = stack.back();
        G4LogicalVolume* currentLog = currentPhys->GetLogicalVolume();
        stack.pop_back();
        // Add all of the current children to the stack.
        for (int child = 0;
             child < currentLog->GetNoDaughters();
             ++child) {
            stack.push_back(currentLog->GetDaughter(child));
        }
        // Add the mass of each element in the current volume.
        double mass = currentLog->GetMass(true,false);
        G4Material *mat = currentLog->GetMaterial();
        materialMass[mat] += mass;
        for (unsigned i = 0; i < mat->GetNumberOfElements(); ++i) {
            const G4Element *elem = mat->GetElement(i);
            for (unsigned j = 0; j < elem->GetNumberOfIsotopes(); ++j) {
                // Find the isotope for this element and create it if it
                // doesn't exist
                std::string isoName = elem->GetIsotope(j)->GetName();
                componentMass[isoName] += mass
                    *(mat->GetFractionVector()[i])
                    *(elem->GetRelativeAbundanceVector()[j]);
            }
        }
    }
    TGeoMixture *theMix
        = new TGeoMixture(averageName.c_str(),
                          componentMass.size(),
                          totalDensity);
    for (std::map<std::string,double>::iterator c = componentMass.begin();
         c != componentMass.end();
         ++c) {
        TGeoElement* elem = fIsotope[c->first];
        theMix->AddElement(elem,c->second/totalMass);
    }
    double radLen = 0;
    double intLen = 0;
    double massSum = 0;
    for (std::map<const G4Material*,double>::iterator
             mat = materialMass.begin();
         mat != materialMass.end(); 
         ++mat) {
        const G4Material* material = mat->first;
        double mass = mat->second;
        massSum += mass;
        radLen += mass/material->GetRadlen();
        intLen += mass/material->GetNuclearInterLength();
    }
    if (massSum>0.0) {
        // The minus sign is to make sure this over-rides the approximate ROOT
        // radiation and interaction length calculations.
        theMix->SetRadLen(-massSum/radLen,-massSum/intLen);
    }
    int numed = fMedium.size() + 1;
    TGeoMedium* theMedium = new TGeoMedium(averageName.c_str(),numed,theMix);
    fMedium[averageName] = theMedium;
    return theMedium;
}
