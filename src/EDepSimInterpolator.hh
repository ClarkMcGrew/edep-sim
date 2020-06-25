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

        double interpolate(const double* point, const std::vector<std::vector<std::vector<double>>>& g,
                           const double* delta, const double* offset) const;
        double interpolate(double x, double y, double z, const std::vector<std::vector<std::vector<double>>>& g,
                           double hx, double hy, double hz, double xo, double yo, double zo) const;

    private:
        double conv_kernel(double s) const;
};

#endif
