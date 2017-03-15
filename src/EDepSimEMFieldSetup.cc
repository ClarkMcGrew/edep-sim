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
/// \file field/field02/src/EDepSimElectricFieldSetup.cc
/// \brief Implementation of the EDepSim::ElectricFieldSetup class
//
//
// $Id: EDepSimEMFieldSetup.cc 77123 2013-11-21 16:13:28Z gcosmo $
//
//   User Field class implementation.
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EDepSimEMFieldSetup.hh"

#include "G4UniformElectricField.hh"
#include "G4UniformMagField.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4EquationOfMotion.hh"
#include "G4EqMagElectricField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"

#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//  Constructors:

EDepSim::EMFieldSetup::EMFieldSetup(G4FieldManager* fieldManager)
    : fFieldManager(fieldManager),
      fChordFinder(0),
      fEquation(0),
      fEMfield(0),
      fStepper(0),
      fIntgrDriver(0),
      fStepperType(4),    // ClassicalRK4 -- the default stepper
      fMinStep(0.010*mm)  // minimal step of 10 microns
{
    fEMfield = new G4UniformElectricField(G4ThreeVector(0.0,500.0*volt/cm,0.0));
    fEquation = new G4EqMagElectricField(fEMfield);

    UpdateField();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EDepSim::EMFieldSetup::EMFieldSetup(G4ElectroMagneticField* field,
                                    G4FieldManager* fieldManager)
    : fFieldManager(fieldManager),
      fChordFinder(0),
      fEquation(0),
      fEMfield(field),
      fStepper(0),
      fIntgrDriver(0),
      fStepperType(4),    // ClassicalRK4 -- the default stepper
      fMinStep(0.010*mm)  // minimal step of 10 microns
{
    fEquation = new G4EqMagElectricField(fEMfield);

    UpdateField();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EDepSim::EMFieldSetup::~EMFieldSetup()
{
    delete fChordFinder;
    delete fStepper;
    delete fEquation;
    delete fEMfield;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EDepSim::EMFieldSetup::UpdateField()
{
    // Register this field to 'global' Field Manager and
    // Create Stepper and Chord Finder with predefined type, minstep (resp.)

    if (!fFieldManager) fFieldManager = GetGlobalFieldManager();

    SetStepper();

    G4cout<<"The minimal step is equal to "<<fMinStep/mm<<" mm"<<G4endl;

    fFieldManager->SetDetectorField(fEMfield);

    if (fChordFinder) delete fChordFinder;

    fIntgrDriver = new G4MagInt_Driver(fMinStep,
                                       fStepper,
                                       fStepper->GetNumberOfVariables());

    fChordFinder = new G4ChordFinder(fIntgrDriver);

    fFieldManager->SetChordFinder(fChordFinder);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EDepSim::EMFieldSetup::SetStepper()
{
// Set stepper according to the stepper type

    G4int nvar = 8;

    if (fStepper) delete fStepper;

    switch ( fStepperType )
    {
    case 0:
        fStepper = new G4ExplicitEuler( fEquation, nvar );
        G4cout<<"G4ExplicitEuler is calledS"<<G4endl;
        break;
    case 1:
        fStepper = new G4ImplicitEuler( fEquation, nvar );
        G4cout<<"G4ImplicitEuler is called"<<G4endl;
        break;
    case 2:
        fStepper = new G4SimpleRunge( fEquation, nvar );
        G4cout<<"G4SimpleRunge is called"<<G4endl;
        break;
    case 3:
        fStepper = new G4SimpleHeum( fEquation, nvar );
        G4cout<<"G4SimpleHeum is called"<<G4endl;
        break;
    case 4:
        fStepper = new G4ClassicalRK4( fEquation, nvar );
        G4cout<<"G4ClassicalRK4 (default) is called"<<G4endl;
        break;
    case 5:
        fStepper = new G4CashKarpRKF45( fEquation, nvar );
        G4cout<<"G4CashKarpRKF45 is called"<<G4endl;
        break;
    case 6:
        fStepper = 0; // new G4RKG3_Stepper( fEquation, nvar );
        G4cout<<"G4RKG3_Stepper is not currently working for Electric Field"
              <<G4endl;
        break;
    case 7:
        fStepper = 0; // new G4HelixExplicitEuler( fEquation );
        G4cout<<"G4HelixExplicitEuler is not valid for Electric Field"<<G4endl;
        break;
    case 8:
        fStepper = 0; // new G4HelixImplicitEuler( fEquation );
        G4cout<<"G4HelixImplicitEuler is not valid for Electric Field"<<G4endl;
        break;
    case 9:
        fStepper = 0; // new G4HelixSimpleRunge( fEquation );
        G4cout<<"G4HelixSimpleRunge is not valid for Electric Field"<<G4endl;
        break;
    default: fStepper = 0;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4FieldManager*  EDepSim::EMFieldSetup::GetGlobalFieldManager()
{
//  Utility method

    return G4TransportationManager::GetTransportationManager()
        ->GetFieldManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
