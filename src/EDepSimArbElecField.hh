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
// class EDepSim::ArbElecField
//
// Class description:
//
// Class for storing and handling an arbitrary electric field.
//
// History:
// - 2020.04.14 A.Cudd created
// -------------------------------------------------------------------

#ifndef EDEPSIMARBELECFIELD_H
#define EDEPSIMARBELECFIELD_H

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "G4Types.hh"
#include "G4ElectricField.hh"
#include "G4SystemOfUnits.hh"

#include "EDepSimLog.hh"
#include "EDepSimInterpolator.hh"

namespace EDepSim { class ArbElecField; }

class EDepSim::ArbElecField : public G4ElectricField
{
    public:
        ArbElecField();

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
