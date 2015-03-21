#ifndef DSimEMField_hh_seen
#define DSimEMField_hh_seen

#include <globals.hh>
#include "G4ElectroMagneticField.hh"

class G4FieldManager;

/// Return the field from the detector magnet at any point is space.  This
/// assumes that the field is limited to the drift region.
class DSimEMField : public G4ElectroMagneticField {

  public:

    DSimEMField(G4double);
    virtual ~DSimEMField() {}

    /// Set the nominal magnetic field strength.
    void SetFieldStrength(G4double);

    /// Get the nominal electric field strength.
    G4double GetFieldStrength(void) const {return fFieldStrength;}

    /// Required method to return the field at a position.
    void GetFieldValue(const G4double Point[4], G4double *Bfield) const;

    /// Required method to tell geant if particle energy is changed.
    G4bool DoesFieldChangeEnergy() const {return true;}

protected:

    // Find the global Field Manager
    G4FieldManager* GetGlobalFieldManager();   // static

  private:

    /// The field strength at the center of the detector.
    G4double fFieldStrength;

};
#endif
