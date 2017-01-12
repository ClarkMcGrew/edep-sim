#ifndef EDepSim_UserDetectorConstruction_h
#define EDepSim_UserDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4Element;
class G4Material;
class G4VPhysicalVolume;
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

    /// Construct the detector and define the world volume.
    virtual G4VPhysicalVolume* Construct();

    /// Return the detector construction messenger
    virtual EDepSim::DetectorMessenger* GetMessenger(void) {
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
    EDepSim::DetectorMessenger* fDetectorMessenger;

    /// A constructor to create the world.
    EDepSim::Builder* fWorldBuilder;

private:

    /// The default material.
    G4Material* fDefaultMaterial;

    /// Apply Validation
    bool fValidateGeometry;
};

#endif
