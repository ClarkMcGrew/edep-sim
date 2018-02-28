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
//
// $Id$
//
// 
//
// Class for creation of uniform Electric Field
//
// 30.1.97 V.Grichine
//
// -------------------------------------------------------------------

#include "EDepSimUniformField.hh"
#include "G4PhysicalConstants.hh"

EDepSim::UniformField::UniformField(
    const G4ThreeVector bField,
    const G4ThreeVector eField )
{
    fFieldComponents[0] = bField.x();
    fFieldComponents[1] = bField.y();
    fFieldComponents[2] = bField.z();
    fFieldComponents[3] = eField.x();
    fFieldComponents[4] = eField.y();
    fFieldComponents[5] = eField.z();
}
   
EDepSim::UniformField::UniformField(
    const G4ThreeVector bField )
{
    fFieldComponents[0] = bField.x();
    fFieldComponents[1] = bField.y();
    fFieldComponents[2] = bField.z();
    fFieldComponents[3] = 0.0;
    fFieldComponents[4] = 0.0;
    fFieldComponents[5] = 0.0;
}
   
EDepSim::UniformField::~UniformField()
{
}

EDepSim::UniformField::UniformField (const EDepSim::UniformField &p)
    : G4ElectricField(p)
{
    for (G4int i=0; i<6; i++) {
        fFieldComponents[i] = p.fFieldComponents[i];
    }
}

EDepSim::UniformField&
EDepSim::UniformField::operator = (const EDepSim::UniformField &p)
{
    for (G4int i=0; i<6; i++) {
        fFieldComponents[i] = p.fFieldComponents[i];
    }
    return *this;
}

// ------------------------------------------------------------------------

void EDepSim::UniformField::GetFieldValue (const G4double* /* position */,
                                           G4double *fieldBandE ) const 
{
    for (G4int i=0; i<6; ++i) {
        fieldBandE[i] = fFieldComponents[i];
    }
}
