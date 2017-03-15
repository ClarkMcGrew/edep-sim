//
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
// class EDepSim::UniformElectricField
//
// Class description:
//
// Class for creation of Uniform electric Magnetic Field.

// History:
// - 30.01.97 V.Grichine, Created.
// -------------------------------------------------------------------

#ifndef G4UNIFORMELECTRICFIELD_HH
#define G4UNIFORMELECTRICFIELD_HH

#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include "G4ElectricField.hh"

namespace EDepSim {class UniformField;}

class EDepSim::UniformField : public G4ElectricField
{
  public:

    /// Define a uniform magnetic field.  The electric field will be set to
    /// zero.  This is equivalent to G4UniformMagneticField().  
    UniformField(const G4ThreeVector bField);

    /// Define a uniform magnetic and electric field.
    UniformField(const G4ThreeVector bField, const G4ThreeVector eField);

    virtual ~UniformField() ;

    // Copy constructor and assignment operator
    UniformField(const UniformField &p);
    UniformField& operator = (const UniformField &p);
    
    /// Provide the field value at a point [x,y,z,t].  The field follows the
    /// G4 standard so that the magnetic field is in field[0], field[1], and
    /// field[2] while the electric field is in field[3], field[3], and
    /// field[5].
    virtual void GetFieldValue(const G4double pos[4], G4double *field) const;

  private:
  
    /// The field components follows the G4 standard so that the magnetic
    /// field is in [0], [1], and [2] while the electric field is in [3], [4],
    /// and [5].
    G4double fFieldComponents[6] ;
};

#endif
