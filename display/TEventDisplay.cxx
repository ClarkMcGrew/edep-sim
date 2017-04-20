#include "TEventDisplay.hxx"
#include "TGUIManager.hxx"
#include "TEventChangeManager.hxx"
#include "TTrajectoryChangeHandler.hxx"
#include "TG4HitChangeHandler.hxx"


#include <TEveManager.h>
#include <TColor.h>
#include <TGLViewer.h>

#include <algorithm>
#include <cmath>
#include <iostream>

EDep::TEventDisplay* EDep::TEventDisplay::fEventDisplay = NULL;

EDep::TEventDisplay& EDep::TEventDisplay::Get(void) {
    if (!fEventDisplay) {
        fEventDisplay = new EDep::TEventDisplay();
        fEventDisplay->Init();
    }
    return *fEventDisplay;
}

EDep::TEventDisplay::TEventDisplay() {}

void EDep::TEventDisplay::Init() {
    TEveManager::Create();

    TGLViewer* glViewer = gEve->GetDefaultGLViewer();
    glViewer->SetCurrentCamera(TGLViewer::kCameraPerspXOZ);
    glViewer->SetGuideState(TGLUtil::kAxesEdge,kTRUE,kFALSE,0);
    glViewer->SetDrawCameraCenter(kTRUE);

    // This is accessed through the GUI() method.
    fGUIManager = new TGUIManager();

    // Create the event display manager.  This needs the GUI, so it has to be
    // done after TGUIManager is created.
    fEventChangeManager = new TEventChangeManager();
    fEventChangeManager->AddUpdateHandler(new TTrajectoryChangeHandler());
    fEventChangeManager->AddUpdateHandler(new TG4HitChangeHandler());

    // Create the color palette.  This is split into two halves.  The first
    // half is from dark to white and is used for negative values on the
    // digitization plots, as well has the reconstruction objects.  The second
    // halve is from white to dark and is used for positive values on the
    // digitization plot.
    Double_t s[]  = { 0.00, 0.20, 0.35, 0.43, 0.45, 0.50, 1.00};
    Double_t r[]  = { 0.00, 0.90, 1.00, 1.00, 1.00, 1.00, 0.00};
    Double_t g[]  = { 0.50, 0.10, 0.75, 0.90, 1.00, 1.00, 0.00};
    Double_t b[]  = { 0.30, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00};
    fColorCount = 200;
    unsigned int abc = sizeof(s)/sizeof(s[0]);
        
    fColorBase = TColor::CreateGradientColorTable(abc, s, r, g, b,
                                                  fColorCount);    
    
    fEnergyPerCharge = 1.0;

    std::cout << "Event display constructed" << std::endl;


}

EDep::TEventDisplay::~TEventDisplay() {
    std::cout << "Event display deconstructed" << std::endl;
}

int EDep::TEventDisplay::LinearColor(double value, double minVal, double maxVal) {
    int nCol = fColorCount/2;
    int iValue = nCol*(value - minVal)/(maxVal - minVal);
    nCol = std::max(0,std::min(iValue,nCol));
    return fColorBase + nCol;
}

int EDep::TEventDisplay::LogColor(double value, double minVal, double maxVal,
    double magScale) {
    int nCol = fColorCount/2;
    double scale = std::pow(10.0,magScale);
    double nvalue = std::max(0.0,std::min((value-minVal)/(maxVal-minVal),1.0));
    double lValue = std::log10(1.0+scale*nvalue)/magScale;
    int iValue = nCol*lValue;
    iValue = std::max(0,std::min(iValue,nCol-1));
    return fColorBase + iValue;
}
