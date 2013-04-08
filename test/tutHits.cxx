#include <cmath>
#include <algorithm>
#include <iostream>

#include <tut.h>
#include <TGeoManager.h>

#include "tutRunSimulation.hxx"

#include "HEPUnits.hxx"
#include "TEvent.hxx"
#include "TG4HitSegment.hxx"
#include "TG4Trajectory.hxx"

namespace tut {
    struct baseHits {
        baseHits() {
            // Run before each test.
            GenerateND280MCEvents("Hits");
        }
        ~baseHits() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseHits>::object testHits;
    test_group<baseHits> groupHits("Hits");

    // Check that hit containers exist for all events.
    template<> template<>
    void testHits::test<1> () {
        int eventsWithHits=0;
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TDataVector> dv
                = (*e)->Get<CP::TDataVector>("truth/g4Hits");
            ensure("Data vector of hit containers exists", dv);
            int hitCount = 0;
            for (CP::TDataVector::iterator d = dv->begin();
                 d != dv->end();
                 ++d) {
                ++hitCount;
                CP::THandle<CP::TG4HitContainer> hitCon
                    = (*d)->Get<CP::TG4HitContainer>(".");
                ensure("Only hit containers in g4Hits",hitCon);
                ensure_greaterthan("Hit containers have contents",
                                   hitCon->size(), (unsigned) 0);
            }
            if (hitCount>0) ++eventsWithHits;
        }
        ensure_greaterthan("Events with hit containers exist",
                           eventsWithHits,
                           1+0.5*detsim::Events.size());
    }

    // Check that hit contributors are in the trajectory container.
    template<> template<>
    void testHits::test<2> () {
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TDataVector> dv=(*e)->Get<CP::TDataVector>("truth/g4Hits");
            CP::THandle<CP::TG4TrajectoryContainer> trajCon 
                = (*e)->Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");
            for (CP::TDataVector::iterator d = dv->begin();
                 d != dv->end();
                 ++d) {
                CP::THandle<CP::TG4HitContainer> hitCon
                    = (*d)->Get<CP::TG4HitContainer>(".");
                for (CP::TG4HitContainer::iterator h = hitCon->begin();
                     h != hitCon->end();
                     ++h) {
                    const CP::TG4HitSegment* hit 
                        = dynamic_cast<const CP::TG4HitSegment*>(*h);
                    CP::THandle<CP::TG4Trajectory> traj
                        = trajCon->GetTrajectory(hit->GetPrimaryId());
                    ensure("Primary trajectory exists", traj);
                    for (std::vector<int>::const_iterator c
                             = hit->GetContributors().begin();
                         c != hit->GetContributors().end();
                         ++c) {
                        CP::THandle<CP::TG4Trajectory> contrib
                            = trajCon->GetTrajectory(*c);
                        ensure("Contributing trajectory exists", contrib);
                    }
                }
            }
        }
    }

    // Check that there is energy deposited and track length for each hit.
    template<> template<>
    void testHits::test<3> () {
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TDataVector> dv=(*e)->Get<CP::TDataVector>("truth/g4Hits");
            for (CP::TDataVector::iterator d = dv->begin();
                 d != dv->end();
                 ++d) {
                CP::THandle<CP::TG4HitContainer> hitCon
                    = (*d)->Get<CP::TG4HitContainer>(".");
                for (CP::TG4HitContainer::iterator h = hitCon->begin();
                     h != hitCon->end();
                     ++h) {
                    const CP::TG4HitSegment* hit 
                        = dynamic_cast<const CP::TG4HitSegment*>(*h);
                    ensure_greaterthan("Hit has energy deposit",
                                       hit->GetEnergyDeposit(),0);
                    ensure_greaterthan("Hit has track length",
                                       hit->GetTrackLength(),0);
                }
            }
        }
    }

    // Check that the hit track length is OK.
    template<> template<>
    void testHits::test<4> () {
        int event = 0;
        double maxDelta = 0.0;
        int longCount = 0;
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TDataVector> dv=(*e)->Get<CP::TDataVector>("truth/g4Hits");
            for (CP::TDataVector::iterator d = dv->begin();
                 d != dv->end();
                 ++d) {
                CP::THandle<CP::TG4HitContainer> hitCon
                    = (*d)->Get<CP::TG4HitContainer>(".");
                double maxSegmentLength = 1.5*unit::cm;
                if (std::string(hitCon->GetName()) != "tpc") {
                    // We don't set a stepping limit for volumes outside of
                    // the TPC, so we can have very long steps (limited by the
                    // actual geometry).  The longest single active geometric
                    // object in the ND280 is about 3 meters long.
                    maxSegmentLength = 300*unit::cm;
                }
                for (CP::TG4HitContainer::iterator h = hitCon->begin();
                     h != hitCon->end();
                     ++h) {
                    const CP::TG4HitSegment* hit 
                        = dynamic_cast<const CP::TG4HitSegment*>(*h);
                    TVector3 start(hit->GetStartX(),
                                   hit->GetStartY(),
                                   hit->GetStartZ());
                    TVector3 stop(hit->GetStopX(),
                                  hit->GetStopY(), 
                                  hit->GetStopZ());
                    double segmentLength = (stop-start).Mag();
                    maxDelta = std::max(maxDelta, 
                                        segmentLength-hit->GetTrackLength());
                    ensure_greaterthan("Track longer than segment length.",
                                       hit->GetTrackLength(),
                                       segmentLength - 0.1*unit::mm);
                    if (segmentLength>maxSegmentLength) {
                        ++longCount;
                        TVector3 dir;
                        dir = dir*(1.0/segmentLength);
                        std::cout << "Long Node Volumes in event: "
                                  << event << std::endl;
                        std::cout << "  Energy: " 
                                  << hit->GetEnergyDeposit() << std::endl;
                        std::cout << "  Length: " 
                                  << hit->GetTrackLength() << std::endl;
                        gGeoManager->FindNode(start.X(), start.Y(), start.Z());
                        std::cout << "  Start Name: " 
                                  << gGeoManager->GetCurrentNode()->GetName()
                                  << std::endl;
                        for (double step=segmentLength/10; 
                             step<0.95*segmentLength; 
                             step += segmentLength/10) {
                            TVector3 pos = start + dir*step;
                            gGeoManager->FindNode(pos.X(), pos.Y(), pos.Z());
                            std::cout << "  Step " << step << " " 
                                      << (gGeoManager
                                          ->GetCurrentNode()->GetName())
                                      << std::endl;
                        }
                        gGeoManager->FindNode(stop.X(), stop.Y(), stop.Z());
                        std::cout << "  Stop Name: " 
                                  << gGeoManager->GetCurrentNode()->GetName()
                                  << std::endl;
                    }
                }
            }
            ++event;
        }
        ensure("No Long Segments", longCount==0);
    }

    // Check that end points of the hit segment are in the same volume.
    template<> template<>
    void testHits::test<5> () {
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TDataVector> dv=(*e)->Get<CP::TDataVector>("truth/g4Hits");
            for (CP::TDataVector::iterator d = dv->begin();
                 d != dv->end();
                 ++d) {
                CP::THandle<CP::TG4HitContainer> hitCon
                    = (*d)->Get<CP::TG4HitContainer>(".");
                for (CP::TG4HitContainer::iterator h = hitCon->begin();
                     h != hitCon->end();
                     ++h) {
                    const CP::TG4HitSegment* hit 
                        = dynamic_cast<const CP::TG4HitSegment*>(*h);
                    TVector3 start(hit->GetStartX(),
                                   hit->GetStartY(),
                                   hit->GetStartZ());
                    TVector3 stop(hit->GetStopX(),
                                  hit->GetStopY(), 
                                  hit->GetStopZ());
                    TVector3 dir = stop-start;
                    double segmentLength = dir.Mag();
                    // Skip really short segments.
                    if (segmentLength<0.3*unit::mm) continue;
                    dir = dir*(1.0/segmentLength);
                    // Figure out how far to step to get away from roundoff
                    // errors.  This needs to be a relatively large step since
                    // some of the hits "scrape a wall".
                    double safetyStep = std::min(0.1*unit::mm,
                                                 segmentLength/4);
                    TVector3 safeStart = start + safetyStep*dir;
                    gGeoManager->FindNode(safeStart.X(),
                                          safeStart.Y(),
                                          safeStart.Z());
                    int startNodeId = gGeoManager->GetCurrentNodeId();
                    ensure("Start node id was found",(startNodeId>=0));
                    std::string startName
                        = gGeoManager->GetCurrentNode()->GetName();
                    TVector3 safeStop = stop - safetyStep*dir;
                    gGeoManager->FindNode(safeStop.X(), 
                                          safeStop.Y(), 
                                          safeStop.Z());
                    int stopNodeId = gGeoManager->GetCurrentNodeId();
                    std::string stopName
                        = gGeoManager->GetCurrentNode()->GetName();
                    ensure("Stop node id was found",(stopNodeId>=0));
                    if (startNodeId != stopNodeId) {
                        std::cout << "Node Id mismatch" << std::endl;
                        std::cout << "  Length " << segmentLength << std::endl;
                        gGeoManager->FindNode(start.X(), start.Y(), start.Z());
                        std::cout << "  Start Name: " 
                                  << gGeoManager->GetCurrentNode()->GetName()
                                  << std::endl;
                        for (double step=safetyStep; 
                             step<10*safetyStep && step<segmentLength/2; 
                             step += safetyStep) {
                            TVector3 pos = start + dir*step;
                            gGeoManager->FindNode(pos.X(), pos.Y(), pos.Z());
                            std::cout << "  Step " << step << " " 
                                      << (gGeoManager
                                          ->GetCurrentNode()->GetName())
                                      << std::endl;
                        }
                        std::cout << "..." << std::endl;
                        for (double step
                                 = std::max(segmentLength-10*safetyStep,
                                            segmentLength/2);
                             step<segmentLength; 
                             step += safetyStep) {
                            TVector3 pos = start + dir*step;
                            gGeoManager->FindNode(pos.X(), pos.Y(), pos.Z());
                            std::cout << "  Step " << step << " " 
                                      << (gGeoManager
                                          ->GetCurrentNode()->GetName())
                                      << std::endl;
                        }
                        gGeoManager->FindNode(stop.X(), stop.Y(), stop.Z());
                        std::cout << "  Stop Name: " << segmentLength
                                  << " " 
                                  << gGeoManager->GetCurrentNode()->GetName()
                                  << std::endl;
                    }
                    ensure("Start and stop nodes are the same",
                           (startNodeId == stopNodeId));
                }
            }
        }
    }
};
