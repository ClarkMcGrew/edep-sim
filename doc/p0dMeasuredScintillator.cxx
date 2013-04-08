/// \page p0dMeasuredScintillatorOutline Define the P0D Scintillator Outline
/// 
/// Define a table of positions outlining the P0D scintillator.  This is take
/// from us-b280-doc number 150.  The file is named SCINT_as_extruded.png.  The
/// outline was digitized using XFig with the outline points being chosen by
/// hand.  The program can be directly compiled by g++ (i.e. g++
/// p0dMeasuredScintillator.cxx) to produce a.out which can be run from the
/// command line. Text that is ready to be placed in an include file is printed
/// to stdout.
///
/// Inside the detSim/doc directory, the include file is regenerated as
/// 
/// \code
/// $ g++ p0dMeasuredScintillator.cxx
/// $ ./a.out > P0DMeasuredScintillator.hxx
/// \endcode
///
/// The program that generates the include file is part of the detSim package. 
///

// This prints a table of corner positions that can be included to define the
// shape of the P0D scintillator. 
//
// The origin is at the top left corner.
#include <iostream>
#include <cmath>

// Y Ruler 8.5 mm: Saved in "xfig coordinates"
const double kP0DScintMeasured_YRuler[2][2] = { {2831,10756},
                                               {2826, 7353}};

// X ruler 33.0 mm: Saved in "xfig coordinates"
const double kP0DScintMeasured_XRuler[2][2] = { {3021, 12454},
                                               {16190, 12443}};

// Scintillator outline: Saved in "xfig coordinates"
const double kP0DScintMeasured_Scintillator[35][2] = {
    {4006, 9996},
    {4680, 9135},
    {6030, 7830},
    {6750, 7110},
    {7591, 6188},
    {8190, 5535},
    {8775, 4905},
    {9086, 4666},
    {9346, 4596},
    {9836, 4606},
    {10157, 4761},
    {10746, 5336},
    {11746, 6406},
    {12414, 7119},
    {13376, 8066},
    {14186, 8876},
    {14886, 9716},
    {15159, 10068},
    {15233, 10269},
    {15116, 10426},
    {14657, 10585},
    {13941, 10627},
    {12266, 10552},
    {11019, 10509},
    {10036, 10446},
    {9263, 10467},
    {8336, 10526},
    {7216, 10566},
    {6074, 10630},
    {5312, 10650},
    {4424, 10624},
    {4060, 10541},
    {3866, 10446},
    {3816, 10246},
    {3946, 10066}};
    
// Coating outline: Saved in "xfig coordinates"
const double kP0DScintMeasured_Coating[19][2] = {
    {3321, 10479},
    {3801, 10012},
    {4735, 8974},
    {6231, 7503},
    {8298, 5303},
    {9556, 4088},
    {10562, 5059},
    {12011, 6559},
    {14264, 8853},
    {15764, 10536},
    {15837, 10656},
    {15722, 10751},
    {11689, 10593},
    {10129, 10559},
    {8698, 10586},
    {5433, 10716},
    {3514, 10736},
    {3253, 10689},
    {3247, 10562}};

int main(int argc, char**argv) {
    double yScale = (kP0DScintMeasured_YRuler[1][1] 
                     - kP0DScintMeasured_YRuler[0][1]);
    double xScale = (kP0DScintMeasured_XRuler[1][0] 
                     - kP0DScintMeasured_XRuler[0][0]);

    xScale = 33.0/xScale;
    yScale = 8.5/yScale;

    double scaledScintillator[35][2];
    for (int i=0; i<35; ++i) {
        scaledScintillator[i][0] = kP0DScintMeasured_Scintillator[i][0]*xScale;
        scaledScintillator[i][1] = kP0DScintMeasured_Scintillator[i][1]*yScale;
    }

    double xMin = 999999;
    double xMax = -999999;
    double yOrigin = 900000;
    double scaledCoating[35][2];
    for (int i=0; i<19; ++i) {
        scaledCoating[i][0] = kP0DScintMeasured_Coating[i][0]*xScale;
        scaledCoating[i][1] = kP0DScintMeasured_Coating[i][1]*yScale;
        yOrigin = std::min(yOrigin, scaledCoating[i][1]);
        xMin = std::min(xMin, scaledCoating[i][0]);
        xMax = std::max(xMax, scaledCoating[i][0]);
    }
    yOrigin = yOrigin + 8.5;
    double xOrigin = 0.5*(xMax+xMin);
    
    for (int i=0; i<35; ++i) {
        scaledScintillator[i][0] -= xOrigin;
        scaledScintillator[i][1] -= yOrigin;
    }
    for (int i=0; i<19; ++i) {
        scaledCoating[i][0] -= xOrigin;
        scaledCoating[i][1] -= yOrigin;
    }
    
    // Put this in the unnamed namespace.
    std::cout 
        << "// DO NOT EDIT -- Generated from p0dMeasuredScintillator.cxx"
        << std::endl
        << "//    The source for p0dMeasuredScintillator.cxx is in the detSim"
        << std::endl
        << "//      documentation subdirectory."
        << std::endl;
    std::cout << "#ifndef P0DMeasuredScintillator_hxx_SEEN" << std::endl;
    std::cout << "#define P0DMeasuredScintillator_hxx_SEEN" << std::endl;
    std::cout << "#include <HEPUnits.hxx>" << std::endl;
    std::cout << "namespace {" << std::endl;
    std::cout << std::endl;
    std::cout << "    // The outline of the P0D scintillator bar." << std::endl;
    std::cout << "    const int kP0DScintMeasured_NCoating = 19;" << std::endl;
    std::cout << "    const double kP0DScintMeasured_Coating[19][2] = {"
              << std::endl;
    for (int i=0; i<19; ++i) {
        std::cout << "        {" << scaledCoating[i][0] << "*unit::mm" 
                  << ", " << scaledCoating[i][1] << "*unit::mm},"
                  << std::endl;
    }
    std::cout << "    };" << std::endl;
    std::cout << std::endl;
    std::cout << "    // The outline of the scintillator inside the bar."
              << std::endl;
    std::cout << "    const int kP0DScintMeasured_NScint = 35;" << std::endl;
    std::cout << "    const double kP0DScintMeasured_Scint[35][2] = {"
              << std::endl;
    for (int i=0; i<35; ++i) {
        std::cout << "        {" << scaledScintillator[i][0] << "*unit::mm" 
                  << ", " << scaledScintillator[i][1] << "*unit::mm},"
                  << std::endl;
    }
    std::cout << "    };" << std::endl << std::endl;
    std::cout << "};" << std::endl;
    std::cout << "#endif" << std::endl;
}
