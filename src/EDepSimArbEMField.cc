// License and Disclaimer
//
// For use with software authored by the Geant4 Collaboration
//
// MIT License
//
// Copyright (c) 2020 Andrew Cudd
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// Class for storing and handling an arbitrary EM field.
//
// History:
// - 2020.04.14 A.Cudd created
// - 2020.07.28 C.McGrew updated license with permission of A.Cudd
//
// -------------------------------------------------------------------

#include "EDepSimArbEMField.hh"

EDepSim::ArbEMField::ArbEMField()
    : efield(nullptr)
    , bfield(nullptr)
{
}

EDepSim::ArbEMField::ArbEMField(G4Field* efield_in, G4Field* bfield_in)
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
