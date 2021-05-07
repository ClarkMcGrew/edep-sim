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
            double energy = g4Hit->GetEnergyDeposit();
            double length = g4Hit->GetTrackLength();
            double dEdX = energy;
            if (length>0.01) dEdX /= length;

            int contrib = g4Hit->GetContributors().front();
            std::string particle = gEDepSimEvent->Trajectories[contrib].GetName();

            TEveLine* eveHit = new TEveLine(2);
            eveHit->SetName(detector->first.c_str());
            std::ostringstream title;
            title << "Hit(" << particle << ")";
            title << std::fixed << std::setprecision(1)
                  << " " << dEdX*1000.0 << " keV/mm";
            title << " for " << length << " mm"
                  << " at (" << g4Hit->GetStart().X() << " mm"
                  << "," <<  g4Hit->GetStart().Y() << " mm"
                  << "," <<  g4Hit->GetStart().Z() << " mm"
                  << ")";

            eveHit->SetTitle(title.str().c_str());
            eveHit->SetLineWidth(5);
            eveHit->SetLineColor(TEventDisplay::Get().LogColor(
                                     dEdX,
                                     minEnergy,
                                     maxEnergy,
                                     3));
            eveHit->SetPoint(0,
                             g4Hit->GetStart().X(),
                             g4Hit->GetStart().Y(),
                             g4Hit->GetStart().Z());
            eveHit->SetPoint(1,
                             g4Hit->GetStop().X(),
                             g4Hit->GetStop().Y(),
                             g4Hit->GetStop().Z());
            hitList->AddElement(eveHit);
        }
        fG4HitList->AddElement(hitList);
    }

}
