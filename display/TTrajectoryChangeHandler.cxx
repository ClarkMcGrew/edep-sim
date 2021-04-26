#include "TTrajectoryChangeHandler.hxx"
#include "TEventDisplay.hxx"
#include "TGUIManager.hxx"

#include <TG4Event.h>
#include <TEventManager.hxx>

#include <TGeoManager.h>
#include <TGButton.h>

#include <TEveManager.h>
#include <TEveLine.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

#include <sstream>
#include <iostream>

EDep::TTrajectoryChangeHandler::TTrajectoryChangeHandler() {
    fTrajectoryList = new TEveElementList("g4Trajectories",
                                          "Geant4 Trajectories");
    fTrajectoryList->SetMainColor(kRed);
    fTrajectoryList->SetMainAlpha(1.0);
    gEve->AddElement(fTrajectoryList);
}

EDep::TTrajectoryChangeHandler::~TTrajectoryChangeHandler() {
}

void EDep::TTrajectoryChangeHandler::Apply() {

    fTrajectoryList->DestroyElements();

    if (!EDep::TEventDisplay::Get().GUI().GetShowTrajectoriesButton()->IsOn()) {
        return;
    }

    std::cout <<"Handle the trajectories" << std::endl;

    for (TG4TrajectoryContainer::iterator traj
             = gEDepSimEvent->Trajectories.begin();
         traj != gEDepSimEvent->Trajectories.end();
         ++traj) {
        const TG4Trajectory::TrajectoryPoints& points = traj->Points;

        std::ostringstream label;
        label << traj->GetName()
              << " (" << traj->GetInitialMomentum().E() << " MeV)";

        if (traj->GetParentId() < 0) {
            std::cout << "Primary " << traj->GetTrackId()
                      << " " << traj->GetName()
                      << " " << traj->GetInitialMomentum().P() << " MeV/c"
                      << " w/ points " << points.size();
            if (!points.empty()) {
                std::cout << " ("
                          << (int)(traj->Points.front().GetPosition().X())
                          << " mm"
                          << ", "
                          << (int)(traj->Points.front().GetPosition().Y())
                          << " mm"
                          << ", "
                          << (int)(traj->Points.front().GetPosition().Z())
                          << " mm)";
            }
            std::cout << std::endl;
        }

        bool charged = false;
        std::string particleClass("none");
        TParticlePDG* particle
            = TDatabasePDG::Instance()->GetParticle(traj->GetPDGCode());
        if (particle) {
            if (std::abs(particle->Charge()) > 0.01) charged = true;
            particleClass = particle->ParticleClass();
        }

        TEveLine *track = new TEveLine();
        track->SetName("trajectory");
        track->SetTitle(label.str().c_str());

        if (particleClass == "Lepton") track->SetLineColor(kMagenta);
        else if (particleClass == "Baryon") track->SetLineColor(kOrange);
        else if (particleClass == "Meson") track->SetLineColor(kGreen);
        else if (particleClass == "GaugeBoson") track->SetLineColor(kCyan);
        else track->SetLineColor(kRed);

        if (charged) track->SetLineStyle(3);
        else track->SetLineStyle(4);

        for (std::size_t p = 0; p < points.size(); ++p) {
            track->SetPoint(p,
                            points[p].GetPosition().X(),
                            points[p].GetPosition().Y(),
                            points[p].GetPosition().Z());
        }
        fTrajectoryList->AddElement(track);
    }
}
