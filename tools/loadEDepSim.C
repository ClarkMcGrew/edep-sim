// TEST MACRO FOR USE WITH OLDER ROOT6.  DOESN"T WORK WHEN CLING KNOWS ABOUT
// THE VARIOUS CLASSES.
#include <TFile.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TStyle.h>

#include <string>
#include <iostream>

/// Build the class definitions needed to read an output tree from edep-sim,
/// and then load them.  This is intended to be run as a command line macro,
/// and is a simple example of how to start reading a edep-sim file.
///
/// For example, to build the shared library and exit, you might run
///
/// root -q edepSimTreeFile.root loadEDepSim.C
///
/// which will create the EDepSimEvents/ directory with the class definitions,
/// and the EDepSimEvents.so shared library.
///
/// If you are running an analysis macro, you might run
///
/// root -q edepSimTreeFile.root loadEDepSim.C myAnalysis.C++
///
/// This will compile and run the code in myAnalysis.C.  There is no reason to
/// compile loadEDepSim.C since it is fast enough for jit.
///
/// The analysis macro should include the headers directly.
///
void loadEDepSim() {
    // Get the directory of this file.
    std::string thisFileDirectory(gSystem->DirName(__FILE__));

    // Add the directory of this file to the macro path.
    std::string macroPath(TROOT::GetMacroPath());
    macroPath = macroPath + thisFileDirectory + ":";
    gROOT->SetMacroPath(macroPath.c_str());

    // Add the current working directory to the include path.
    gSystem->AddIncludePath(("-I"+gSystem->GetWorkingDirectory()).c_str());

    // Add the directory of this file to the macro path.
    gSystem->AddIncludePath(("-I"+thisFileDirectory).c_str());

    // Set the color palette used for continuous scales.
    gStyle->SetPalette(kCMYK);

    // Make sur we have a file.  If we don't, then stop.
    if (!gFile) {
        std::cout << "ERROR: The loadEDepSim macro needs to have a file loaded."
                  << std::endl;
        std::cout << "  e.g. root -l input.root loadEDepSim.C"
                  << std::endl;
        return;
    }
    
    // Dynamically load the event tree definition
    gFile->MakeProject("EDepSimEvents","*","recreate++");
    // Add the directory of this file to the macro path.
    gSystem->AddIncludePath(("-I"+thisFileDirectory+"/EDepSimEvents").c_str());

    // Get the geometry.
    gFile->Get("EDepSimGeometry");

    // Load the rest of the tools.
    gROOT->ProcessLine(".x readEDepSim.C+");
    gROOT->ProcessLine(".x plotEDepSim.C+");

}
