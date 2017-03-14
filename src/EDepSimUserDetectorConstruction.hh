#ifndef EDepSim_UserDetectorConstruction_h
#define EDepSim_UserDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4Element;
class G4Material;
class G4VPhysicalVolume;
class G4GDMLParser;
namespace EDepSim {class Builder;}
namespace EDepSim {class UserDetectorConstruction;}

#include "EDepSimDetectorMessenger.hh"
#include "EDepSimBuilder.hh"

/// Construct the EDepSim:: detector geometry.
namespace EDepSim {class UserDetectorConstruction;}
class EDepSim::UserDetectorConstruction : public G4VUserDetectorConstruction {
public:
    UserDetectorConstruction();
    virtual ~UserDetectorConstruction();

    /// The required method to construct the detector and define the world
    /// volume.
    virtual G4VPhysicalVolume* Construct();

    /// The method to setup the sensitive detectors and fields.  In a multi
    /// thread application, this is called per thread.
    virtual void ConstructSDandField();
    
    /// Return the detector construction messenger
    virtual EDepSim::DetectorMessenger* GetMessenger(void) {
        return fDetectorMessenger;
    };

    /// Update the geometry information to match stuff read from the macro
    /// file.  
    void UpdateGeometry();

    /// Set ValidateGeomtry to true
    void ValidateGeometry() {fValidateGeometry = true;}

    /// Set the GDML parser that this class should use.
    void SetGDMLParser(G4GDMLParser* parser) {fGDMLParser = parser;}

    /// Get the GDML parser that this class is using.
    G4GDMLParser* GetGDMLParser() {return fGDMLParser;}
    
protected:

    /// Define the materials used in the detector.
    void DefineMaterials(void);

    /// Define the natural isotope abundance.
    G4Element* DefineElement(G4String name, G4String symbol, G4double z);

    /// This really constructs the detector, but doesn't define
    /// materials before it's constructed.  This is called by Construct()
    G4VPhysicalVolume* ConstructDetector();

    /// A messenger to for the DetectorConstruction object.
    EDepSim::DetectorMessenger* fDetectorMessenger;

    /// A constructor to create the world.
    EDepSim::Builder* fWorldBuilder;

    /// A GDML Parser if one has been defined.
    G4GDMLParser* fGDMLParser;

private:

    /// The default material.
    G4Material* fDefaultMaterial;

    /// Apply Validation
    bool fValidateGeometry;
};

#endif
