#ifndef DSimMagneticField_hh_seen
#define DSimMagneticField_hh_seen

#include <globals.hh>
#include "G4MagneticField.hh"

class G4FieldManager;

/// Return the field from the DSim magnet at any point is space.  This
/// assumes that the magnet is centered at the origin.

class DSimMagneticField : public G4MagneticField {

  public:

    DSimMagneticField(G4double);
    virtual ~DSimMagneticField() {}

    /// Set the nominal magnetic field strength
    /// at the center of the DSim magnet.
    void SetFieldStrength(G4double);

    /// Get the nominal magnetic field strength
    /// at the center of the DSim magnet.
    G4double GetFieldStrength(void) const {return fFieldStrength;}

    /// Required method to return the field at a position.
    void GetFieldValue(const G4double Point[4], G4double *Bfield) const;

    /// Set the outer dimensions of the yoke
    void SetYokeOuter(G4double width, G4double height, G4double length) {
        fYokeOuterWidth = width;
        fYokeOuterHeight = height;
        fYokeOuterLength = length;
    }

    /// Set the inner dimensions of the yoke
    void SetYokeInner(G4double width, G4double height, G4double length) {
        fYokeInnerWidth = width;
        fYokeInnerHeight = height;
        fYokeInnerLength = length;
    }

    /// Set the outer dimensions of the coil
    void SetCoilOuter(G4double width, G4double height, G4double length) {
        fCoilOuterWidth = width;
        fCoilOuterHeight = height;
        fCoilOuterLength = length;
    }

    /// Set the inner dimensions of the coil
    void SetCoilInner(G4double width, G4double height, G4double length) {
        fCoilInnerWidth = width;
        fCoilInnerHeight = height;
        fCoilInnerLength = length;
    }

  protected:

    // Find the global Field Manager
    G4FieldManager* GetGlobalFieldManager();   // static

  private:

    /// The field strength at the center of the magnet.
    G4double fFieldStrength;

    /// The outer width of the magnet yoke.
    G4double fYokeOuterWidth;

    /// The outer height of the magnet yoke.
    G4double fYokeOuterHeight;

    /// The outer length of the magnet yoke.
    G4double fYokeOuterLength;

    /// The inner width of the magnet yoke.
    G4double fYokeInnerWidth;

    /// The inner height of the magnet yoke.
    G4double fYokeInnerHeight;

    /// The inner length of the magnet yoke.
    G4double fYokeInnerLength;

    /// The outer width of the coil
    G4double fCoilOuterWidth;

    /// The outer height of the coil
    G4double fCoilOuterHeight;

    /// The outer length of the coil
    G4double fCoilOuterLength;

    /// The inner width of the coil
    G4double fCoilInnerWidth;

    /// The inner height of the coil
    G4double fCoilInnerHeight;

    /// The inner length of the coil
    G4double fCoilInnerLength;

};
#endif
