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
// Class for routines for interpolating data on a regular grid.
//
// History:
// - 2020.04.14 A.Cudd created
//
// -------------------------------------------------------------------

#include "EDepSimInterpolator.hh"

EDepSim::Bicubic::Bicubic()
{
}

double EDepSim::Bicubic::interpolate(const double* point, const std::vector<std::vector<std::vector<double>>>& g,
                   const double* delta, const double* offset) const
{
    return interpolate(point[0], point[1], point[2], g, delta[0], delta[1], delta[2], offset[0], offset[1], offset[2]);
}

double EDepSim::Bicubic::interpolate(double x, double y, double z, const std::vector<std::vector<std::vector<double>>>& g,
                                     double hx, double hy, double hz, double xo, double yo, double zo) const
{
    double v = 0;

    const int xsize = g.size();
    const int ysize = g[0].size();
    const int zsize = g[0][0].size();

    const double xp = (x - xo) / hx;
    const double yp = (y - yo) / hy;
    const double zp = (z - zo) / hz;

    const int x_idx = std::floor(xp);
    const int y_idx = std::floor(yp);
    const int z_idx = std::floor(zp);

    for(auto i : {x_idx - 1, x_idx, x_idx + 1, x_idx + 2})
    {
        for(auto j : {y_idx - 1, y_idx, y_idx + 1, y_idx + 2})
        {
            for(auto k : {z_idx - 1, z_idx, z_idx + 1, z_idx + 2})
            {
                if(i < 0 || j < 0 || k < 0)
                    continue;

                if(i >= xsize || j >= ysize || k >= zsize)
                    continue;

                const double x_c = conv_kernel(xp - i);
                const double y_c = conv_kernel(yp - j);
                const double z_c = conv_kernel(zp - k);
                v += g[i][j][k] * x_c * y_c * z_c;
            }
        }
    }

    return v;
}

double EDepSim::Bicubic::conv_kernel(double s) const
{
    double v = 0;
    double z = std::abs(s);

    if(0 <= z && z < 1)
        v = 1 + (0.5) * z * z * (3 * z - 5);

    else if(1 < z && z < 2)
        v = 2 - z * (4 + 0.5 * z * (z - 5));

    return v;
}
