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
// class EDepSim::Interpolator
//
// Class description:
//
// Class for routines for interpolating data on a regular grid.
//
// History:
// - 2020.04.14 A.Cudd created
// -------------------------------------------------------------------

#ifndef EDEPSIMINTERPOLATOR_HH
#define EDEPSIMINTERPOLATOR_HH

#include <cmath>
#include <iostream>
#include <vector>

namespace EDepSim {class Cubic;}

class EDepSim::Cubic
{
    public:
        Cubic();

        double interpolate(
            const double* point,
            const std::vector<std::vector<std::vector<double>>>& g,
            const double* delta, const double* offset) const;
        double interpolate(
            double x, double y, double z,
            const std::vector<std::vector<std::vector<double>>>& g,
            double hx, double hy, double hz, double xo, double yo, double zo)
            const;

    private:
        double conv_kernel(double s) const;
};

#endif
