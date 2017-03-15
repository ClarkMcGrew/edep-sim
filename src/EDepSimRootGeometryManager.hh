////////////////////////////////////////////////////////////
// $Id: EDepSim::RootGeometryManager.hh,v 1.13 2011/01/17 02:45:46 mcgrew Exp $
// 
#ifndef EDepSim_RootGeometryManager_hh_Seen
#define EDepSim_RootGeometryManager_hh_Seen

#include "EDepSimVolumeId.hh"

#include <vector>
#include <map>
#include <set>

#include <G4String.hh>
#include <G4ThreeVector.hh>
#include <G4Color.hh>

#include <Rtypes.h>

class TGeoManager;
class TGeoVolume;
class TGeoShape;
class TGeoMatrix;
class TGeoMedium;
class TGeoElement;
class TGeoNode;

class G4Material;
class G4VPhysicalVolume;

/// Provide a root output for the geant 4 events.
namespace EDepSim {class RootGeometryManager;}
class EDepSim::RootGeometryManager {
    typedef struct {
        int color;
        int fill;
        float alpha;
    } DrawAttributes;
    typedef std::map<G4String,DrawAttributes> AttributeMap;

public:
    virtual ~RootGeometryManager();

    /// If a persistency manager has not been created, create one.
    static EDepSim::RootGeometryManager* Get(void);

    /// Export the geometry to a file.
    void Export(const char *file);

    /// Update the root geometry to match the g4 geometry
    void Update(const G4VPhysicalVolume* aWorld, bool validate);

    /// Set the root geometry from a gdml file.
    void Update(std::string gdmlFile, bool validate);
    
    /// Make sure that the current geometry passes a bunch of tests.
    void Validate();
    
    /// Get a volume ID base on the volume position.
    int GetNodeId(const G4ThreeVector& pos);

    /// Set material color
    void SetDrawAtt(G4Material* material, int color, double opacity=1.0);

    /// Get a color for a material.  This can be used to create the
    /// G4VisAttributes object.
    G4Color GetG4Color(G4Material* material);

protected:
    /// use Get() instead
    RootGeometryManager();

    /// Find the right color for a physical volume.  This won't necessarily be
    /// the same color as set in G4, but should be a reasonable choice for a
    /// detector picture.  This will return a negative number if the volume
    /// should be invisible.
    int GetColor(const G4VPhysicalVolume* vol);

    /// Find the fill style for a physical volume.
    int GetFill(const G4VPhysicalVolume* vol);

private:
    /// The pointer to the instantiation of this object.
    static EDepSim::RootGeometryManager* fThis;

    /// A map between G4 material names and Root Material definitions.
    std::map<G4String,TGeoMedium*> fMedium;

    /// A map between G4 isotope names and Root Element definitions.
    std::map<G4String,TGeoElement*> fIsotope;

    /// A map between G4 volume names and count of volumes with that name.
    std::map<G4String,int> fNodeCount;

    /// A map between a material name and a color.
    AttributeMap fColorMap;

    /// A vector of volume names to print
    std::vector<G4String> fPrintMass;

    /// A map of which masses have been printed.
    std::set<G4String> fPrintedMass;

    /// A stack of volume names that have been seen.  These are the "short"
    /// volume names of all of the parents to the current volue.
    std::vector<G4String> fNameStack;

    /// Create a new ROOT shape object based on the G4 solid.  This might be
    /// called recursively if a G4 boolean solid is found.
    TGeoShape* CreateShape(const G4VSolid* theSolid, TGeoMatrix **mat = NULL);
    
    /// Create a new ROOT volume object.
    TGeoVolume* CreateVolume(TGeoManager* theEnvelope,
                             const G4VSolid* theSolid,
                             std::string theName,
                             TGeoMedium* theMedium);

    /// Save the detector envelope.  This is called recursively to fill in the
    /// entire detector.  The G4 physical volume, theVol, is used to create a
    /// new root TGeoVolume which is added to the existing root TGeoVolume,
    /// theMother, as a daughter.  
    ///
    /// RETURN VALUE: This returns true if one of the immediate daughter
    /// volumes was ignored.  This tells the mother volume that it will need
    /// to adjust it's mass and material to account for the missing detail.
    bool CreateEnvelope(const G4VPhysicalVolume* theVol,
                        TGeoManager* theEnvelope,
                        TGeoVolume* theMother);

    // Method counts how many nodes there are in mother volume with the
    // same name as daughter node.
    int HowManySimilarNodesInVolume(TGeoVolume* theMother,
                                    std::string theName);

    /// Create the materials needed for the detector.  This called recursively
    /// to find all of the materials in the detector.
    void CreateMaterials(const G4VPhysicalVolume * theVol);

    /// A method to prune the geometry that gets exported from G4 into root.
    /// If this returns true, the volume and all of it's children will not be
    /// exported to ROOT.  This allows the internal structure of low level
    /// objects (i.e. the internal "structure" of scintillating bars) to be
    /// hidden.  
    virtual bool IgnoreVolume(const G4VPhysicalVolume* theVol);

    /// A method to flag that a volume mass should be printed.
    virtual bool PrintMass(const G4VPhysicalVolume* theVol);

    /// A method to request that a volume mass should be printed;
    virtual void ShouldPrintMass(std::string name) {
        fPrintMass.push_back(name);
    }

    /// A method to translate from the local volume material to the right
    /// material for the root geometry.  This is required for volumes where
    /// the inner structure is not saved, and the material of the outer volume
    /// does not provide a good base name.
    virtual std::string MaterialName(const G4VPhysicalVolume *theVol);

    /// Create a medium that describes the average composition for all the
    /// materials in the logical volume.  This is used when the geometry is
    /// simplified by not including sub-volume (i.e. when the internal
    /// structure of the scintillating bars is skipped).  The averaged
    /// material name is based on the name provided in materialBase;
    virtual TGeoMedium* AverageMaterial(const G4VPhysicalVolume* theVol);
};
#endif
