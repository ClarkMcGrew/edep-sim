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
// Class for storing and handling an arbitrary electric field.
//
// History:
// - 2020.04.14 A.Cudd created
// - 2020.07.28 C.McGrew updated license with permission of A.Cudd
//
// -------------------------------------------------------------------

#include "EDepSimArbElecField.hh"

EDepSim::ArbElecField::ArbElecField()
{
}

bool EDepSim::ArbElecField::ReadFile(const std::string& fname)
{
    m_filename = fname;
    std::fstream fin(fname, std::fstream::in);

    if(!fin.is_open())
    {
        EDepSimError("Can't read " << fname << std::endl);
        return false;
    }
    else
    {
        EDepSimLog("Reading " << fname << " ...");
        int xcount{-1}, ycount{-1}, zcount{-1};
        double xcurr{0}, ycurr{0}, zcurr{0};
        std::string line;

        while(std::getline(fin >> std::ws, line))
        {
            std::stringstream ss(line);

            if(ss.str().front() == '#')
                continue;

            ss >> m_offset[0] >> m_offset[1] >> m_offset[2]
                >> m_delta[0] >> m_delta[1] >> m_delta[2];
            break;
        }

        while(std::getline(fin >> std::ws, line))
        {
            double x{0}, y{0}, z{0}, fx{0}, fy{0}, fz{0}, f{0};
            std::stringstream ss(line);

            if(ss.str().front() == '#')
                continue;

            ss >> x >> y >> z >> fx >> fy >> fz >> f;

            if(std::abs(x - xcurr) > 0.0 || xcount < 0)
            {
                xcurr = x;
                xcount += 1;
                ycount = -1;
                m_field_x.emplace_back(std::vector<std::vector<double>>{});
                m_field_y.emplace_back(std::vector<std::vector<double>>{});
                m_field_z.emplace_back(std::vector<std::vector<double>>{});
            }

            if(std::abs(y - ycurr) > 0.0 || ycount < 0)
            {
                ycurr = y;
                ycount += 1;
                zcount = -1;
                m_field_x[xcount].emplace_back(std::vector<double>{});
                m_field_y[xcount].emplace_back(std::vector<double>{});
                m_field_z[xcount].emplace_back(std::vector<double>{});
            }

            m_field_x[xcount][ycount].push_back(fx * (volt/cm));
            m_field_y[xcount][ycount].push_back(fy * (volt/cm));
            m_field_z[xcount][ycount].push_back(fz * (volt/cm));

            if(std::abs(z - zcurr) > 0.0 || zcount < 0)
            {
                zcurr = z;
                zcount += 1;
            }
        }
    }

    return true;
}

void EDepSim::ArbElecField::GetFieldValue(const G4double pos[4], G4double* field) const
{
    const double x = pos[0];
    const double y = pos[1];
    const double z = pos[2];

    EDepSim::Cubic interp;
    field[3] = interp.interpolate(x, y, z, m_field_x, m_delta[0], m_delta[1], m_delta[2], m_offset[0], m_offset[1], m_offset[2]);
    field[4] = interp.interpolate(x, y, z, m_field_y, m_delta[0], m_delta[1], m_delta[2], m_offset[0], m_offset[1], m_offset[2]);
    field[5] = interp.interpolate(x, y, z, m_field_z, m_delta[0], m_delta[1], m_delta[2], m_offset[0], m_offset[1], m_offset[2]);
}

void EDepSim::ArbElecField::PrintInfo() const
{
    EDepSimLog("Printing values for electric field.");
    EDepSimLog("m_filename : " << m_filename
              << "\nm_offset   : " << m_offset[0] << ", " << m_offset[1] << ", " << m_offset[2]
              << "\nm_delta    : " << m_delta[0] << ", " << m_delta[1] << ", " << m_delta[2]);
}
