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
// Class for storing and handling an arbitrary magnetic field.
//
// History:
// - 2020.04.14 A.Cudd created
//
// -------------------------------------------------------------------

#include "EDepSimArbMagField.hh"

EDepSim::ArbMagField::ArbMagField()
{
}

bool EDepSim::ArbMagField::ReadFile(const std::string& fname)
{
    m_filename = fname;
    std::fstream fin(fname, std::fstream::in);

    if(!fin.is_open())
    {
        //std::cout << "[ERROR]: Can't read " << fname << std::endl;
        EDepSimError("Can't read " << fname << std::endl);
        return false;
    }
    else
    {
        //std::cout << "Reading " << fname << " ..." << std::endl;
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

            m_field_x[xcount][ycount].push_back(fx * tesla);
            m_field_y[xcount][ycount].push_back(fy * tesla);
            m_field_z[xcount][ycount].push_back(fz * tesla);

            if(std::abs(z - zcurr) > 0.0 || zcount < 0)
            {
                zcurr = z;
                zcount += 1;
            }
        }
    }

    return true;
}

void EDepSim::ArbMagField::GetFieldValue(const G4double pos[4], G4double* field) const
{
    const double x = pos[0];
    const double y = pos[1];
    const double z = pos[2];

    EDepSim::Bicubic interp;
    field[0] = interp.interpolate(x, y, z, m_field_x, m_delta[0], m_delta[1], m_delta[2], m_offset[0], m_offset[1], m_offset[2]);
    field[1] = interp.interpolate(x, y, z, m_field_y, m_delta[0], m_delta[1], m_delta[2], m_offset[0], m_offset[1], m_offset[2]);
    field[2] = interp.interpolate(x, y, z, m_field_z, m_delta[0], m_delta[1], m_delta[2], m_offset[0], m_offset[1], m_offset[2]);
}

void EDepSim::ArbMagField::PrintInfo() const
{
    //std::cout << "Printing values for magnetic field." << std::endl;
    //std::cout << "m_filename : " << m_filename << std::endl
    //          << "m_offset   : " << m_offset[0] << ", " << m_offset[1] << ", " << m_offset[2] << std::endl
    //          << "m_delta    : " << m_delta[0] << ", " << m_delta[1] << ", " << m_delta[2] << std::endl;
    EDepSimLog("Printing values for magnetic field.");
    EDepSimLog("m_filename : " << m_filename
              << "\nm_offset   : " << m_offset[0] << ", " << m_offset[1] << ", " << m_offset[2]
              << "\nm_delta    : " << m_delta[0] << ", " << m_delta[1] << ", " << m_delta[2]);
}
