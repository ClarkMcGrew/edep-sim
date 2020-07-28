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
// class EDepSim::ArbMagField
//
// Class description:
//
// Class for storing and handling an arbitrary magnetic field.
//
// History:
// - 2020.04.14 A.Cudd created
// - 2020.07.28 C.McGrew updated license with permission of A.Cudd
//
// -------------------------------------------------------------------

#ifndef EDEPSIMARBMAGFIELD_H
#define EDEPSIMARBMAGFIELD_H

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "G4Types.hh"
#include "G4MagneticField.hh"
#include <G4SystemOfUnits.hh>

#include "EDepSimLog.hh"
#include "EDepSimInterpolator.hh"

namespace EDepSim { class ArbMagField; }

class EDepSim::ArbMagField : public G4MagneticField
{
    public:
        ArbMagField();

        bool ReadFile(const std::string& fname);
        void PrintInfo() const;
        virtual void GetFieldValue(const G4double pos[4], G4double* field) const;

    private:
        std::string m_filename;
        std::array<double, 3> m_offset;
        std::array<double, 3> m_delta;
        std::vector<std::vector<std::vector<double>>> m_field_x;
        std::vector<std::vector<std::vector<double>>> m_field_y;
        std::vector<std::vector<std::vector<double>>> m_field_z;
};

#endif
