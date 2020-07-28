// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file field/field02/include/EDepSimEMFieldSetup.hh
/// \brief Definition of the EDepSim::EMFieldSetup class
//
//
// $Id: EDepSim::EMFieldSetup.hh 76247 2013-11-08 11:18:52Z gcosmo $
//
// History:
//    - 2017.03.15 C.McGrew adapted for EDepSim
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EDepSimEMFieldSetup_h
#define EDepSimEMFieldSetup_h 1

#include <G4ThreeVector.hh>

class G4FieldManager;
class G4ChordFinder;
class G4ElectroMagneticField;
class G4EquationOfMotion;
class G4Mag_EqRhs;
class G4EqMagElectricField;
class G4MagIntegratorStepper;
class G4MagInt_Driver;

namespace EDepSim {class EMFieldSetup;}

/// A class for control of the Electric Field of the detector.
///
/// The field for this case is uniform.
/// It is simply a 'setup' class that creates the field and necessary
/// other parts
class EDepSim::EMFieldSetup
{
public:
    /// Create a new field.  The first argument is a general EM field (for
    /// example, G4UniformMagneticField, or G4UniformElectricField.  The
    /// second argument is the field manager to setup.
    EMFieldSetup(G4ElectroMagneticField* field, G4FieldManager* m=0);

    /// Create a zero field for the field manager.
    EMFieldSetup(G4FieldManager* m=0);

    virtual ~EMFieldSetup();

    void SetStepperType(G4int i) { fStepperType = i ; }

    void SetStepper();

    void SetMinStep(G4double s) { fMinStep = s ; }

protected:

    // Find the global Field Manager

    G4FieldManager*         GetGlobalFieldManager();

    void UpdateField();

private:

    G4FieldManager*         fFieldManager;

    G4ChordFinder*          fChordFinder;

    G4EqMagElectricField*   fEquation;

    G4ElectroMagneticField* fEMfield;

    G4MagIntegratorStepper* fStepper;
    G4MagInt_Driver*        fIntgrDriver;

    G4int                   fStepperType;

    G4double                fMinStep;
};
#endif
