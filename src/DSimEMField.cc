#include "DSimEMField.hh"
#include "DSimException.hh"

#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>

#include <DSimLog.hh>

DSimEMField::DSimEMField(G4double f) 
    : fFieldStrength(f) {
    if (fFieldStrength != 0.0) GetGlobalFieldManager()->SetDetectorField(this);
    else GetGlobalFieldManager()->SetDetectorField(NULL);
}

void DSimEMField::SetFieldStrength(G4double s) {
    fFieldStrength = s;

    // Find the Field Manager for the global field
    G4FieldManager* fieldMgr= GetGlobalFieldManager();

    if (fFieldStrength != 0.) {
       fieldMgr->SetDetectorField(this);
    } 
    else {
        // If the new field's value is zero, then it is best to
        // insure that it is not used for propagation.
        fieldMgr->SetDetectorField(NULL);
    }
}

void DSimEMField::GetFieldValue(const double point[4],
                                double *Bfield ) const {
    G4ThreeVector pos(point[0], point[1], point[2]);

    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;

    // from a measured field map.
    Bfield[3] = 0.0;
    Bfield[4] = 0.0;
    Bfield[5] = fFieldStrength;

    return;
}

G4FieldManager* DSimEMField::GetGlobalFieldManager()
{
  return G4TransportationManager::GetTransportationManager()->GetFieldManager();
}

