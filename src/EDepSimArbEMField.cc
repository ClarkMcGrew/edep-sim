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
// Class for storing and handling an arbitrary EM field.
//
// History:
// - 2020.04.14 A.Cudd created
//
// -------------------------------------------------------------------

#include "EDepSimArbEMField.hh"

EDepSim::ArbEMField::ArbEMField()
    : efield(nullptr)
    , bfield(nullptr)
{
}

EDepSim::ArbEMField::ArbEMField(G4ElectroMagneticField* efield_in, G4ElectroMagneticField* bfield_in)
    : efield(efield_in)
    , bfield(bfield_in)
{
}

EDepSim::ArbEMField::ArbEMField(const EDepSim::ArbEMField& cpy)
    : EDepSim::ArbEMField(cpy.efield, cpy.bfield)
{
    efield = cpy.efield;
    bfield = cpy.bfield;
}

EDepSim::ArbEMField& EDepSim::ArbEMField::operator=(const ArbEMField& rhs)
{
    if(&rhs == this)
        return *this;
    else
    {
        efield = rhs.efield;
        bfield = rhs.bfield;
        return *this;
    }
}

EDepSim::ArbEMField::~ArbEMField()
{
    delete bfield;
    delete efield;
}

void EDepSim::ArbEMField::GetFieldValue(const G4double pos[4], G4double *field) const
{
    //GetFieldValue always expects an array with six elements for both the EField
    //and BField, even if it only fills one of the fields.
    double tmp_bfield[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double tmp_efield[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    if(bfield != nullptr)
        bfield->GetFieldValue(pos, tmp_bfield);

    if(efield != nullptr)
        efield->GetFieldValue(pos, tmp_efield);

    //Geant4 convention is the first three elements are Bx, By, Bz
    //and the next three are Ex, Ey, Ez
    field[0] = tmp_bfield[0];
    field[1] = tmp_bfield[1];
    field[2] = tmp_bfield[2];

    field[3] = tmp_efield[3];
    field[4] = tmp_efield[4];
    field[5] = tmp_efield[5];
}
