#ifndef DSimUserDetectorConstruction_h
#define DSimUserDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4Element;
class G4Material;
class G4VPhysicalVolume;

#include "DSimDetectorMessenger.hh"

class DSimBuilder; 

/// Construct the DSim detector geometry.
class DSimUserDetectorConstruction : public G4VUserDetectorConstruction {
public:
    DSimUserDetectorConstruction();
    virtual ~DSimUserDetectorConstruction();

    /// Construct the detector and define the world volume.
    virtual G4VPhysicalVolume* Construct();

    /// Return the detector construction messenger
    virtual DSimDetectorMessenger* GetMessenger(void) {
        return fDetectorMessenger;
    };

    /// Update the geometry information to match stuff read from the macro
    /// file.  
    void UpdateGeometry();

    /// Set ValidateGeomtry to true
    void ValidateGeometry() {fValidateGeometry = true;}

protected:

    /// Define the materials used in the detector.
    void DefineMaterials(void);

    /// Define the natural isotope abundance.
    G4Element* DefineElement(G4String name, G4String symbol, G4double z);

    /// This really constructs the detector, but doesn't define
    /// materials before it's constructed.  This is called by Construct()
    G4VPhysicalVolume* ConstructDetector();

    /// A messenger to for the DetectorConstruction object.
    DSimDetectorMessenger* fDetectorMessenger;

    /// A constructor to create the world.
    DSimBuilder* fWorldBuilder;

private:

    /// The default material.
    G4Material* fDefaultMaterial;

    /// Apply Validation
    bool fValidateGeometry;
};

#endif
