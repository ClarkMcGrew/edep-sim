#include "TG4HitChangeHandler.hxx"
#include "TEventDisplay.hxx"
#include "TGUIManager.hxx"

#include <TG4Event.h>
#include <TEventManager.hxx>

#include <TGButton.h>

#include <TEveManager.h>
#include <TEveLine.h>

#include <sstream>
#include <iostream>
#include <iomanip>

EDep::TG4HitChangeHandler::TG4HitChangeHandler() {
    fG4HitList = new TEveElementList("g4HitList","Geant4 Truth Hits");
    fG4HitList->SetMainColor(kCyan);
    fG4HitList->SetMainAlpha(1.0);
    gEve->AddElement(fG4HitList);
}

EDep::TG4HitChangeHandler::~TG4HitChangeHandler() {
}

void EDep::TG4HitChangeHandler::Apply() {

    fG4HitList->DestroyElements();

    if (!EDep::TEventDisplay::Get().GUI().GetShowG4HitsButton()->IsOn()) {
        std::cout << "G4 hits disabled" << std::endl;
        return;
    }

    std::cout << "Handle the geant4 truth hits" << std::endl;
    if (!gEDepSimEvent) return;

    double minEnergy = 0.18;
    double maxEnergy = 3.0;

    for (TG4HitSegmentDetectors::iterator detector
             = gEDepSimEvent->SegmentDetectors.begin();
         detector != gEDepSimEvent->SegmentDetectors.end();
         ++detector) {
        std::cout << "Show hits for " << detector->first
                  << " (" << detector->second.size() << " hits)"<< std::endl;
        TEveElementList* hitList
            = new TEveElementList(
                detector->first.c_str(),
                ("Energy Deposit for " + detector->first).c_str());
        for (TG4HitSegmentContainer::iterator g4Hit = detector->second.begin();
             g4Hit != detector->second.end();
             ++g4Hit) {
            double energy = g4Hit->EnergyDeposit;
            double length = g4Hit->TrackLength;
            double dEdX = energy;
            if (length>0.01) dEdX /= length;

            TEveLine* eveHit = new TEveLine(2);
            eveHit->SetName(detector->first.c_str());
            std::ostringstream title;
            title << "G4 Hit";
            title << std::fixed << std::setprecision(2)
                  << " " << dEdX << " MeV/mm";
            title << " for " << length << " mm"
                  << " at (" << g4Hit->Start.X() << " mm"
                  << "," <<  g4Hit->Start.Y() << " mm"
                  << "," <<  g4Hit->Start.Z() << " mm"
                  << ")";

            eveHit->SetTitle(title.str().c_str());

            eveHit->SetLineColor(TEventDisplay::Get().LogColor(
                                     dEdX,
                                     minEnergy,
                                     maxEnergy,
                                     3));
            eveHit->SetPoint(0,
                             g4Hit->Start.X(),
                             g4Hit->Start.Y(),
                             g4Hit->Start.Z());
            eveHit->SetPoint(1,
                             g4Hit->Stop.X(),
                             g4Hit->Stop.Y(),
                             g4Hit->Stop.Z());
            hitList->AddElement(eveHit);
        }
        fG4HitList->AddElement(hitList);
    }

}
