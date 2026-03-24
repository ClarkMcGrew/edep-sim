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

namespace EDepSim {class UserDetectorConstruction;}

/// Construct the EDepSim detector geometry.  This handles two methods of
/// construction.  In the first, the geometry is read from a GDML file which
/// is expected to contain the SensDet, EField and BField auxiliary types for
/// logical volumes that are sensitive, have an electric field, and have a
/// magnetic field (respectively).  The alternative is to define a builder
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

    /// Exclude a logical volume from being a sensitive detector (e.g. the
    /// Rock around the detector).  This is used to override a gdml geometry.
    void AddExcludedSensitiveDetector(std::string exclude) {
        fExcludeAsSensitiveDetector.push_back(exclude);
    }

    class UserUpdateGeometryAction {
    public:
        UserUpdateGeometryAction() {};
        virtual ~UserUpdateGeometryAction() {}

        /// Notify the class that the geometry has changed.  This must not
        /// change the state of Geant4 or EDepSim.
        virtual void UpdateGeometry(const G4VPhysicalVolume* world) {};

    };

    /// Add a external user action to be called when the geometry has been
    /// updated.  pre and post actions.  The external action can collect
    /// information about the geometry, but must not modify the state of G4,
    /// or EDepSim.
    void AddExternalAction(
        EDepSim::UserDetectorConstruction::UserUpdateGeometryAction* action) {
        fExternalActions.push_back(action);
    }

protected:

    /// Define the materials used in the detector. This is only used when the
    /// ConstructDetector method is used.
    void DefineMaterials(void);

    /// DEPRECATED: Define the natural isotope abundance. The NIST elements
    /// should be used for most things.
    G4Element* DefineElement(G4String name, G4String symbol, G4double z);

    /// This really constructs the detector to build a hard-coded geometry.
    /// It is mostly used during testing when there isn't a GDML input being
    /// used.
    G4VPhysicalVolume* ConstructDetector();

    /// A messenger to for the DetectorConstruction object.
    EDepSim::DetectorMessenger* fDetectorMessenger;

    /// A constructor to create the world.  This is mostly used for testing
    /// when GDML is not being used.
    EDepSim::Builder* fWorldBuilder;

    /// A GDML Parser if one has been defined.
    G4GDMLParser* fGDMLParser;

    /// The constructed world volume.
    G4VPhysicalVolume* fPhysicalWorld;

private:

    /// Apply Validation
    bool fValidateGeometry;

    /// Vector of logical volumes to exclude being sensitive detectors.
    std::vector<std::string> fExcludeAsSensitiveDetector;

    /// Vector of update actions that need to be called.
    std::vector<EDepSim::UserDetectorConstruction::UserUpdateGeometryAction*>
    fExternalActions;
};

#endif
