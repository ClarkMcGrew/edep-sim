// * License and Disclaimer                                           *
//
// MIT License

// Copyright (c) 2020 Andrew Cudd

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Class for routines for interpolating data on a regular grid.
//
// History:
// - 2020.04.14 A.Cudd created
//
// -------------------------------------------------------------------

#include "EDepSimInterpolator.hh"

EDepSim::Cubic::Cubic()
{
}

double EDepSim::Cubic::interpolate(const double* point, const std::vector<std::vector<std::vector<double>>>& g,
                   const double* delta, const double* offset) const
{
    return interpolate(point[0], point[1], point[2], g, delta[0], delta[1], delta[2], offset[0], offset[1], offset[2]);
}

double EDepSim::Cubic::interpolate(double x, double y, double z, const std::vector<std::vector<std::vector<double>>>& g,
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

double EDepSim::Cubic::conv_kernel(double s) const
{
    double v = 0;
    double z = std::abs(s);

    if(0 <= z && z < 1)
        v = 1 + (0.5) * z * z * (3 * z - 5);

    else if(1 < z && z < 2)
        v = 2 - z * (4 + 0.5 * z * (z - 5));

    return v;
}
