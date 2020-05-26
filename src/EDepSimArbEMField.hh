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
// class EDepSim::ArbEMField
//
// Class description:
//
// Class for storing and handling an arbitrary EM field.
//
// History:
// - 2020.04.14 A.Cudd created
// -------------------------------------------------------------------

#ifndef EDEPSIMARBEMFIELD_H
#define EDEPSIMARBEMFIELD_H

#include "G4Types.hh"
#include "G4ElectroMagneticField.hh"

namespace EDepSim { class ArbEMField; }

class EDepSim::ArbEMField : public G4ElectroMagneticField
{
    public:
        ArbEMField();
        ArbEMField(G4ElectroMagneticField* efield_in, G4ElectroMagneticField* bfield_in);

        ArbEMField(const ArbEMField& cpy);
        ArbEMField& operator=(const ArbEMField& rhs);

        virtual ~ArbEMField();

        virtual void GetFieldValue(const G4double pos[4], G4double *field) const;
        virtual G4bool DoesFieldChangeEnergy() const { return true; };

        void SetEField(G4ElectroMagneticField* efield_in) { efield = efield_in; };
        void SetBField(G4ElectroMagneticField* bfield_in) { bfield = bfield_in; };

    private:
        G4ElectroMagneticField* efield;
        G4ElectroMagneticField* bfield;
};

#endif
