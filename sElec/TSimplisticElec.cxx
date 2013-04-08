#include <map>
#include <cstdlib>

#include <TVector3.h>
#include <TH2F.h>

#include <oaEvent_version.h>

#include <TND280Event.hxx>
#include <THandle.hxx>
#include <TG4HitSegment.hxx>
#include <TManager.hxx>
#include <TMCHit.hxx>
#include <TND280Log.hxx>

#if oaEvent_MAJOR_VERSION > 6
#include <TGeomIdManager.hxx>
#include <ND280GeomId.hxx>
#include <TGeometryId.hxx>
#else
#warning Simplistic electronics simulator requires oaEvent version 7 or later.
#endif

#include "TSimplisticElec.hxx"

SElec::TSimplisticElec::TSimplisticElec(std::string g4Hits, 
                                        std::string hits, 
                                        double maxStep,
                                        TH2* depositHist)
    : fG4Hits(g4Hits), fHits(hits), fEnergyMultiplier(1.0),
      fMaxStep(maxStep), fDepositHist(depositHist) {}

SElec::TSimplisticElec::~TSimplisticElec() {}

void SElec::TSimplisticElec::GenerateHits(CP::TND280Event& event) {
#if oaEvent_MAJOR_VERSION > 6
    CP::THandle<CP::TG4HitContainer> g4Hits = 
        event.Get<CP::TG4HitContainer>("truth/g4Hits/"+fG4Hits);

    if (!g4Hits) return;

    CP::TGeometryId geomId;
    std::map<CP::TGeometryId,CP::TWritableMCHit> mappedHits;
    std::map<CP::TGeometryId,double> mappedLength;
    for (CP::TG4HitContainer::iterator g4Hit = g4Hits->begin();
         g4Hit != g4Hits->end();
         ++g4Hit) {
        CP::TG4HitSegment* hitSeg = dynamic_cast<CP::TG4HitSegment*>(*g4Hit);
        if (!hitSeg) continue;

        TVector3 start(hitSeg->GetStartX(), 
                       hitSeg->GetStartY(), 
                       hitSeg->GetStartZ());
        TVector3 stop(hitSeg->GetStopX(), 
                      hitSeg->GetStopY(), 
                      hitSeg->GetStopZ());
                
        double totalLength = (start-stop).Mag();
        int steps = int(totalLength/fMaxStep) + 1;
        double stepFraction = 1.0/steps;
        double stepLength = totalLength*stepFraction;

        for (int s = 0; s<steps; ++s) {
            double step = stepFraction*(s+0.5);
            
            // Find the geometry identifier for this hit.
            double x = (1.0-step)*hitSeg->GetStartX() 
                + step*hitSeg->GetStopX();
            double y = (1.0-step)*hitSeg->GetStartY() 
                + step*hitSeg->GetStopY();
            double z = (1.0-step)*hitSeg->GetStartZ() 
                + step*hitSeg->GetStopZ();
            
            CP::TManager::Get().GeomId().GetGeometryId(x,y,z,geomId);
            std::string volumeName = geomId.GetName();
            
            // Check that this is a sensitive volume (most relevant for the
            // TPC).
            bool goodVolume = true;
            for (std::vector<std::string>::iterator s = fVolumeNames.begin();
                 s != fVolumeNames.end();
                 ++s) {
                if (volumeName.find(*s) == std::string::npos) {
                    goodVolume = false;
                    break;
                }
            }
            if (!goodVolume) continue;
            
            // Add this info to the TMCHit.
            CP::TWritableMCHit& mcHit = mappedHits[geomId];
            if (mcHit.GetGeomId() == CP::GeomId::Empty()) {
                mcHit.SetGeomId(geomId);
                mcHit.SetTime(10*unit::second);
                mappedLength[geomId] = 0.0;
            }
            
            mappedLength[geomId] += stepLength;
            mcHit.SetCharge(mcHit.GetCharge() 
                            + stepFraction*hitSeg->GetEnergyDeposit());
            if (hitSeg->GetStartT()<mcHit.GetTime()) {
                mcHit.SetTime(hitSeg->GetStartT());
            }
        }
    }
    if (mappedHits.empty()) return;
    
    CP::THitSelection* hits = new CP::THitSelection(fHits.c_str(),
                                                    "Truth Hits");

    for (std::map<CP::TGeometryId,CP::TWritableMCHit>::iterator h
             = mappedHits.begin();
         h != mappedHits.end();
         ++h) {
        hits->push_back(CP::THandle<CP::TMCHit>(new CP::TMCHit(h->second)));
        if (fDepositHist) {
            double length = mappedLength[h->first];
            double energy = h->second.GetCharge();
            fDepositHist->Fill(length,energy);
        }
    }
    event.Get<CP::TDataVector>("hits")->push_back(hits);
#else
    ND280Error("Cannot run -- Requires oaEvent version 7 or later");
    std::cout << "NOT RUN" << std::cout;
    std::exit(0);
#endif
}
