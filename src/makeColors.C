// This is a macro to dump the ROOT colors to a include file suitable for
// detSim.
#include <TROOT.h>
#include <TColor.h>
#include <TList.h>
#include <TObject.h>

#include <iostream>

void makeColors() {
    const TColor* color = gROOT->GetColor(kBlue);
    TIter next(gROOT->GetListOfColors());

    std::cout << "// Automatically generated map from the ROOT color index to"
              << std::endl
              << "// a G4Color.  All of the alpha components are set to 1.0"
              << std::endl
              << "// This requires that the map include file is available."
              << std::endl;
    std::cout << "static std::map<int,G4Color> sRootColorToG4ColorMap;"
              << std::endl;
    std::cout << "if (sRootColorToG4ColorMap.empty()) {"
              << std::endl;
    TObject *obj;
    while ((obj = next())) {
        TColor* color = dynamic_cast<TColor*>(obj);
        std::cout << "    sRootColorToG4ColorMap[" << color->GetNumber()
                  << "] = G4Color(" << color->GetRed()
                  << "," << color->GetGreen()
                  << "," << color->GetBlue()
                  << "," << 1.0 << ");"
                  << std::endl;
    }
    std::cout << "}"
              << std::endl;
}
