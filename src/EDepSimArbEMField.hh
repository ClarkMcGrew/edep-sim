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
// class EDepSim::ArbEMField
//
// Class description:
//
// Class for storing and handling an arbitrary EM field.
//
// History:
// - 2020.04.14 A.Cudd created
// - 2020.07.28 C.McGrew updated license with permission of A.Cudd
//
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
        ArbEMField(G4Field* efield_in, G4Field* bfield_in);

        ArbEMField(const ArbEMField& cpy);
        ArbEMField& operator=(const ArbEMField& rhs);

        virtual ~ArbEMField();

        virtual void GetFieldValue(const G4double pos[4], G4double *field) const;
        virtual G4bool DoesFieldChangeEnergy() const { return true; };

        void SetEField(G4Field* efield_in) { efield = efield_in; };
        void SetBField(G4Field* bfield_in) { bfield = bfield_in; };

    private:
        G4Field* efield;
        G4Field* bfield;
};

#endif
