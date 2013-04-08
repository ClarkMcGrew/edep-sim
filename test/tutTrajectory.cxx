#include <algorithm>
#include <iostream>
#include <tut.h>

#include "tutRunSimulation.hxx"

#include <TGeoManager.h>

#include <HEPUnits.hxx>
#include <TEvent.hxx>
#include <TG4Trajectory.hxx>
#include <TG4PrimaryVertex.hxx>

namespace tut {
    struct baseTrajectory {
        baseTrajectory() {
            // Run before each test.
            GenerateND280MCEvents("Trajectory");
        }
        ~baseTrajectory() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseTrajectory>::object testTrajectory;
    test_group<baseTrajectory> groupTrajectory("Trajectory");

    // Check that trajectories exist for all events.
    template<> template<>
    void testTrajectory::test<1> () {
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TG4TrajectoryContainer> trajCon 
                = (*e)->Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");
            ensure("Trajectory container exists", trajCon);
            int trajCount = 0;
            for (CP::TG4TrajectoryContainer::iterator t = trajCon->begin();
                 t != trajCon->end();
                 ++t) {
                ensure_equals("Trajectory key matchs ID",
                              t->first,
                              t->second.GetTrackId());
                ++trajCount;
            }
            ensure_greaterthan("Trajectories exist",trajCount,0);
        }
    }

    // Check that the primary vertices exist.
    template<> template<>
    void testTrajectory::test<2> () {
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TG4PrimaryVertexContainer> primCon
                = (*e)->Get<CP::TG4PrimaryVertexContainer>("truth/G4PrimVertex00");
            ensure("Primary vertices exist", primCon);
            ensure_greaterthan("At least one primary vertex",
                               primCon->size(),0);
            for (CP::TG4PrimaryVertexContainer::const_iterator v=primCon->begin();
                 v != primCon->end();
                 ++v) {
                ensure_greaterthan("At least one primary particle",
                                   v->GetPrimaryParticles().size(),0);
            }
        }
    }

    // Check that parent trajectories exist for all trajectories
    template<> template<>
    void testTrajectory::test<3> () {
        int singleParticleEvents = 0;
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TG4PrimaryVertexContainer> primCon
                = (*e)->Get<CP::TG4PrimaryVertexContainer>("truth/G4PrimVertex00");
            ensure("Primary vertices exist", primCon);
            if (primCon->size()!=1) continue;
            if (primCon->front().GetPrimaryParticles().size()!=1) continue;
            ++singleParticleEvents;
            CP::THandle<CP::TG4TrajectoryContainer> trajCon 
                = (*e)->Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");
            int primaryParticleCount = 0;
            for (CP::TG4TrajectoryContainer::iterator t = trajCon->begin();
                 t != trajCon->end();
                 ++t) {
                if (t->second.GetParentId() == 0) {
                    ++primaryParticleCount;
                    continue;
                }
                CP::TG4TrajectoryContainer::iterator p = 
                    trajCon->find(t->second.GetParentId());
                ensure("Parent trajectory found", p != trajCon->end());
            }
            ensure_equals("Primary particle count",primaryParticleCount,1);
        }
        ensure_greaterthan("Single particle events exist",
                           singleParticleEvents,0);
    }

    // Check that trajectory points have monotonicly decreasing momentum.
    template<> template<>
    void testTrajectory::test<4> () {
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TG4TrajectoryContainer> trajCon 
                = (*e)->Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");
            for (CP::TG4TrajectoryContainer::iterator t = trajCon->begin();
                 t != trajCon->end();
                 ++t) {
                CP::TG4Trajectory::Points& points 
                    = t->second.GetTrajectoryPoints();
                double lastMomentum = points.front().GetMomentum().Mag();
                for (CP::TG4Trajectory::Points::iterator p = points.begin();
                     p != points.end();
                     ++p) {
                    double thisMomentum = p->GetMomentum().Mag();
                    ensure_lessthan("Momentum decreases ", 
                                    thisMomentum, lastMomentum+10*unit::eV);
                    thisMomentum = lastMomentum;
                }
            }
        }
    }

    // Check that the gamma threshold is allowing low energy gamma-rays to be
    // saved.
    template<> template<>
    void testTrajectory::test<5> () {
        // Event 9 is a 1 GeV positron with a 5MeV gamma cut.
        CP::TEvent* lowThreshold = detsim::Events[9];

        // Check that low energy gamma-rays are being saved if the gamma
        // threshold is set to 5 MeV.
        CP::THandle<CP::TG4TrajectoryContainer> lowCon 
            = lowThreshold->Get<CP::TG4TrajectoryContainer>(
                "truth/G4Trajectories");
        int lowCount = 0;
        for (CP::TG4TrajectoryContainer::iterator t = lowCon->begin();
             t != lowCon->end();
             ++t) {
            if (t->second.GetParticleName() != "gamma") continue;
            double mom = t->second.GetInitialMomentum().Vect().Mag();
            if (mom<20*unit::MeV) ++lowCount;
        }
        ensure_greaterthan("Low momentum cut gamma trajectories exist",
                           lowCount, 0);
    }

    // Check that a 50 MeV gamma threshold is properly applied for secondary
    // particles.
    template<> template<>
    void testTrajectory::test<6> () {
        // Event 9 is a 1 GeV positron with a 50MeV gamma cut.
        CP::TEvent* highThreshold = detsim::Events[8];

        CP::THandle<CP::TG4TrajectoryContainer> highCon 
            = highThreshold->Get<CP::TG4TrajectoryContainer>(
                "truth/G4Trajectories");
        int highCount = 0;
        for (CP::TG4TrajectoryContainer::iterator t = highCon->begin();
             t != highCon->end();
             ++t) {
            if (t->second.GetParticleName() != "gamma") continue;
            ++highCount;
            double mom = t->second.GetInitialMomentum().Vect().Mag();
            ensure_greaterthan("Only high momentum gammas exist",
                               mom, 50*unit::MeV);
        }
        ensure_greaterthan("High momentum cut gamma trajectories exist",
                           highCount, 0);

    }


    // Check that the gamma-ray trajectory code is working.
    template<> template<>
    void testTrajectory::test<7> () {
        CP::TEvent* event = detsim::Events[9];

        CP::THandle<CP::TG4TrajectoryContainer> trajCon 
            = event->Get<CP::TG4TrajectoryContainer>(
                "truth/G4Trajectories");
        for (CP::TG4TrajectoryContainer::iterator t = trajCon->begin();
             t != trajCon->end();
             ++t) {
            if (t->second.GetParticleName() != "gamma") continue;
            CP::TG4Trajectory::Points& points = t->second.GetTrajectoryPoints();
            ensure_greaterthan("Gamma trajectory end points exist",
                            points.size(),1);
        }
    }

    // Check that all of the volumes for the trajectory points exist.
    template<> template<>
    void testTrajectory::test<8> () {
        for (detsim::EventVector::iterator e = detsim::Events.begin();
             e != detsim::Events.end(); 
             ++e) {
            CP::THandle<CP::TG4TrajectoryContainer> trajCon 
                = (*e)->Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");
            for (CP::TG4TrajectoryContainer::iterator t = trajCon->begin();
                 t != trajCon->end();
                 ++t) {
                CP::TG4Trajectory::Points& points 
                    = t->second.GetTrajectoryPoints();
                for (CP::TG4Trajectory::Points::iterator p = points.begin();
                     p != points.end();
                     ++p) {
                    TLorentzVector pos = p->GetPosition();
                    gGeoManager->FindNode(pos.X(),pos.Y(),pos.Z());
                    int nodeId = gGeoManager->GetCurrentNodeId();
                    ensure("Node ID was found",(nodeId>=0));
                }
            }
        }
    }
};
