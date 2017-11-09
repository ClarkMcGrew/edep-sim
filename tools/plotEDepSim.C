// TEST MACRO FOR USE WITH OLDER ROOT6.  DOESN"T WORK WHEN CLING KNOWS ABOUT
// THE VARIOUS CLASSES.
#include <loadEDepSim.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TPad.h>
#include <TColor.h>
#include <TDatabasePDG.h>

#include <TGeoManager.h>


#include "plotEDepSim.h"

#include <iostream>


// Dummy.  Could be used if initialization is required.
void plotEDepSim() {}

namespace {
    std::vector<TObject*> plottedTrajectories;
    std::vector<TObject*> plottedHits;
};

/// Clear out all of the plotted trajectories.
void EDepSimClearTrajectories() {
    for (auto elem = plottedTrajectories.begin();
         elem != plottedTrajectories.end();
         ++elem) {
        if (*elem) delete *elem;
    }
    plottedTrajectories.clear();
}

void EDepSimClearHits() {
    for (auto elem = plottedHits.begin();
         elem != plottedHits.end();
         ++elem) {
        if (*elem) delete *elem;
    }
    plottedHits.clear();
}

void EDepSimClearAll() {
    EDepSimClearTrajectories();
    EDepSimClearHits();
}

/// Translate the value to a normalized scale.  This has the same arguments as
/// EDepSimLogScale so that it's easy to change between the two.
double EDepSimLinearScale(double value, double range=1.0, double orders=1.0) {
    if (value < 0.0) return 0.0;
    value = value / range;
    if (value > 1.0) value = 1.0;
    return value;
}

/// Translate the value to a log normalized scale.  This has the same
/// arguments as EDepSimLogScale so that it's easy to change between the two.
double EDepSimLogScale(double value, double scale=10.0, double orders=2.0) {
    if (value < 0.0) return 0.0;
    value = std::log10(10.0*value/scale + 1.0);
    value = value / orders;
    if (value > 1.0) value = 1.0;
    return value;
}

/// Return a linear scaled color.  The color value must be between 0.0 and
/// 1.0.
int EDepSimLinearColor(double color, double range=1.0, double orders=1.0) {
    color = EDepSimLinearScale(color,range,orders);
    int iColor = TColor::GetNumberOfColors()*color;
    iColor = std::min(iColor,TColor::GetNumberOfColors());
    return TColor::GetColorPalette(iColor);
}

/// Return a log scaled color.  The range of zero to "scale" covers about
/// 1/order of the color range.
int EDepSimLogColor(double color, double scale=10.0, double orders=3.0) {
    std::cout << color;
    color = EDepSimLogScale(color,scale,orders);
    std::cout << " " << color;
    std::cout << " " << scale;
    std::cout << " " << orders;
    int iColor = TColor::GetNumberOfColors()*color/orders;
    std::cout << " " << iColor;
    std::cout << "/" << TColor::GetNumberOfColors();
    iColor = std::min(iColor,TColor::GetNumberOfColors());
    std::cout << " " << iColor;
    iColor = TColor::GetColorPalette(iColor);
    std::cout << " " << iColor;
    std::cout << endl;
    return iColor; 
}

namespace {
    TDatabasePDG* pdgDatabase = NULL;
};
int EDepSimParticleColor(int pid) {
    if (!pdgDatabase) pdgDatabase = new TDatabasePDG();
    TParticlePDG* part = pdgDatabase->GetParticle(pid);
    if (!part) return 11;
    std::string particleName(part->GetName());
    std::string particleClass(part->ParticleClass());
    if (particleClass == "Lepton") {
        if (particleName.find("e") != std::string::npos) return kRed;
        if (particleName.find("mu") != std::string::npos) return kMagenta;
        return kGreen-10;
    }
    else if (particleClass == "Meson") {
        if (part->Mass() > 0.200) return kGray;
        if (part->Charge() > 0.0) return kYellow-6;
        if (part->Charge() < 0.0) return kGreen-3;
    }
    else if (particleClass == "Baryon") {
        if (part->Charge() > 1.0) return kBlue + 1;
        if (part->Charge() < 1.0) return kCyan - 3;
        return kCyan - 10;
    }
    return kYellow;
}

int EDepSimParticleStyle(int pid) {
    if (!pdgDatabase) pdgDatabase = new TDatabasePDG();
    TParticlePDG* part = pdgDatabase->GetParticle(pid);
    if (!part) return 11;
    if (std::abs(part->Charge()) > 1.0) return 1;
    return 3;
}

TObject* EDepSimFillTrajectory(int trackId) {
    TG4Event* event = EDepSimEvent();
    TPolyLine3D* line = NULL;
    for (std::vector<TG4Trajectory>::iterator
             trajectory = event->Trajectories.begin();
         trajectory != event->Trajectories.end();
         ++trajectory) {
        if (trajectory->TrackId != trackId) continue;
        std::cout << " Found Trajectory " << trackId
                  << " with " << trajectory->Points.size() << " Points"
                  << std::endl;
        line = new TPolyLine3D(trajectory->Points.size());
        line->SetLineColor(EDepSimParticleColor(trajectory->PDGCode));
        line->SetLineStyle(EDepSimParticleStyle(trajectory->PDGCode));
        int i = 0;
        for (std::vector<TG4TrajectoryPoint>::iterator
                 point = trajectory->Points.begin();
             point != trajectory->Points.end();
             ++point) {
            line->SetPoint(i++,
                           point->Position.X(),
                           point->Position.Y(),
                           point->Position.Z());
        }
        return line;
    }
    return NULL;
}

TObject* EDepSimPlotTrajectory(int trackId) {
    TObject* traj = EDepSimFillTrajectory(trackId);
    if (!traj) return NULL;
    plottedTrajectories.push_back(traj);
    traj->Draw();
    return traj;
}

int EDepSimPlotTrajectories() {
    TG4Event* event = EDepSimEvent();
    int count = 0;
    for (auto track = event->Trajectories.begin();
         track != event->Trajectories.end();
         ++track) {
        if (!EDepSimPlotTrajectory(track->TrackId)) continue;
        ++count;
    }
    gPad->Update();
    return count;
}

int EDepSimPlotSegmentDetector(std::string name) {
    TG4Event* event = EDepSimEvent();
    for (auto detector = event->SegmentDetectors.begin();
         detector != event->SegmentDetectors.end();
         ++detector) {
        if (detector->first != name) continue;
        std::cout << " Found detector " << name
                  << " with " << detector->second.size() << " Hits"
                  << std::endl;
        if (detector->second.size() < 1) return 0;
        for (auto hit = detector->second.begin();
             hit != detector->second.end();
             ++hit) {
            double length = (hit->Stop.Vect()-hit->Start.Vect()).Mag();
            int iColor = EDepSimLogColor(hit->EnergyDeposit,0.4,2.0);
            if (length > 2.0) {
                TPolyLine3D* line = new TPolyLine3D(2);
                line->SetLineColor(iColor);
                line->SetLineStyle(1);
                line->SetPoint(0,
                               hit->Start.X(),
                               hit->Start.Y(),
                               hit->Start.Z());
                line->SetPoint(1,
                               hit->Stop.X(),
                               hit->Stop.Y(),
                               hit->Stop.Z());
                line->Draw();
                plottedHits.push_back(line);
            }
            else {
                TPolyMarker3D* mark = new TPolyMarker3D(1);
                mark->SetMarkerColor(iColor);
                mark->SetMarkerStyle(kCircle);
                mark->SetMarkerSize(hit->TrackLength/2.0);
                TLorentzVector pos = 0.5*(hit->Start + hit->Stop);
                mark->SetPoint(0, pos.X(), pos.Y(), pos.Z());
                mark->Draw();
                plottedHits.push_back(mark);
            }                
        }
        return detector->second.size();
    }
    return 0;
}

int EDepSimPlotSegmentDetectors() {
    TG4Event* event = EDepSimEvent();
    for (auto detector = event->SegmentDetectors.begin();
         detector != event->SegmentDetectors.end();
         ++detector) {
        EDepSimPlotSegmentDetector(detector->first);
    }
    gPad->Update();
    return event->SegmentDetectors.size();
}

void EDepSimPlotEvent(int i = -1, bool drawGeom = false) {
    if (i>=0) EDepSimEvent(i);
    if (drawGeom) gGeoManager->GetTopVolume()->Draw();
    EDepSimPlotTrajectories();
    EDepSimPlotSegmentDetectors();
}
