#include "TG4PhotonHitChangeHandler.hxx"
#include "TEventDisplay.hxx"
#include "TGUIManager.hxx"
#include "EDepSimUnits.h"

#include <TG4Event.h>
#include <TG4PhotonHit.h>
#include <TEventManager.hxx>

#include <TGButton.h>
#include <TEveManager.h>
#include <TEvePointSet.h>

#include <sstream>
#include <iostream>

EDep::TG4PhotonHitChangeHandler::TG4PhotonHitChangeHandler() {
    fPhotonList = new TEveElementList("photonHitList", "Optical Photon Endpoints");
    fPhotonList->SetMainColor(kYellow);
    fPhotonList->SetMainAlpha(1.0);
    gEve->AddElement(fPhotonList);
}

EDep::TG4PhotonHitChangeHandler::~TG4PhotonHitChangeHandler() {}

void EDep::TG4PhotonHitChangeHandler::Apply() {
    fPhotonList->DestroyElements();

    if (!EDep::TEventDisplay::Get().GUI().GetShowPhotonHitsButton()->IsOn()) {
        std::cout << "Photon hits disabled" << std::endl;
        return;
    }

    if (!gEDepSimEvent) return;

    std::cout << "Handle optical photon hits" << std::endl;

    // Two point sets: detected (sensor, yellow) and undetected (lost, blue)
    TEvePointSet* detected   = new TEvePointSet("detected");
    detected->SetTitle("Photons reaching a sensor");

    detected->SetMarkerColor(kYellow);
    detected->SetMarkerStyle(4);   // open circle
    detected->SetMarkerSize(0.5);

    for (auto& det :  gEDepSimEvent->PhotonDetectors) {
        std::cout << "  PhotonDetector: " << det.first
                  << " with " << det.second.size() << " photons" << std::endl;
        int stride = 1 + det.second.size()/2000;
        int print = 1 + det.second.size()/100;

        int entry = 0;
        int count = 0;
        for (auto& ph : det.second) {
            if ((++entry)%print == 0) {
                std::cout << "photon " << entry
                          << std::fixed << std::setprecision(2)
                          << " time:" << ph.GetStop().T()/EDepSim::ns << " ns"
                          << " wavelength: "
                          << ph.GetWavelength()/EDepSim::nm << " nm"
                          << " pos: (" << ph.GetStop().X()/EDepSim::mm
                          << "," << ph.GetStop().Y()/EDepSim::mm
                          << "," << ph.GetStop().Z()/EDepSim::mm << ") mm"
                          << std::endl;
            }

            if (entry%stride != 0) continue;
            ++count;

            detected->SetNextPoint(ph.GetStop().X(),
                                   ph.GetStop().Y(),
                                   ph.GetStop().Z());
        }
        std::cout << "  Photons Shown: " << count
                  << " out of " << entry << std::endl;
    }

    fPhotonList->AddElement(detected);
}
