#include <map>
#include <cstdlib>

#include <TVector3.h>
#include <TH1.h>

#include <TEvent.hxx>
#include <THandle.hxx>
#include <TG4HitSegment.hxx>
#include <TManager.hxx>
#include <TMCHit.hxx>
#include <TCaptLog.hxx>

#include <TGeomIdManager.hxx>
#include <CaptGeomId.hxx>
#include <TGeometryId.hxx>

#include "TSimplisticElec.hxx"

SElec::TSimplisticElec::TSimplisticElec(std::string g4Hits, 
                                        std::string hits, 
                                        double maxStep,
                                        TH1* depositHist)
    : fG4Hits(g4Hits), fHits(hits), fEnergyMultiplier(1.0),
      fMaxStep(maxStep), fDepositHist(depositHist) {}

SElec::TSimplisticElec::~TSimplisticElec() {}

void SElec::TSimplisticElec::GenerateHits(CP::TEvent& event) {
    CP::THandle<CP::TG4HitContainer> g4Hits = 
        event.Get<CP::TG4HitContainer>("truth/g4Hits/"+fG4Hits);

    if (!g4Hits) {
        std::cout << "No hits in events" << std::endl;
        return;
    }
        

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
            
            // Check if this volume should be skipped.
            bool skipVolume = false;
            for (std::vector<std::string>::iterator s = fSkipNames.begin();
                 s != fSkipNames.end();
                 ++s) {
                if (volumeName.find(*s) != std::string::npos)  {
                    skipVolume = true;
                    break;
                }
            }
            if (skipVolume) continue;
            
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
    if (mappedHits.empty()) {
        std::cout << "empty hits" << std::endl;
        return;
    }
    
    CP::THitSelection* hits = new CP::THitSelection(fHits.c_str(),
                                                    "Truth Hits");

    for (std::map<CP::TGeometryId,CP::TWritableMCHit>::iterator h
             = mappedHits.begin();
         h != mappedHits.end();
         ++h) {
        hits->push_back(CP::THandle<CP::TMCHit>(new CP::TMCHit(h->second)));
        double length = mappedLength[h->first];
        double energy = h->second.GetCharge();
        std::cout << " length " << length
                  << "  energy " << energy
                  << std::endl;
        if (fDepositHist && length>0.0) {
            double dEdX = energy/length;
            fDepositHist->Fill(dEdX,length);
        }
    }
    event.Get<CP::TDataVector>("hits")->push_back(hits);
}
