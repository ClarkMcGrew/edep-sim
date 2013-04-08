#include <iostream>
#include <vector>
#include <string>

#include <tut.h>

#include <TGeoManager.h>
#include <TGeoShape.h>
#include <TGeoBBox.h>

#include <HEPUnits.hxx>
#include <TManager.hxx>
#include <TEvent.hxx>
#include <TG4Trajectory.hxx>
#include <TG4PrimaryVertex.hxx>

#include "tutRunSimulation.hxx"

namespace detsim {
    std::vector<int> p0dXBarNodes;
    std::vector<int> p0dYBarNodes;

    void RecurseGeometry(int depth, std::string name) {
        TGeoNode* node = gGeoManager->GetCurrentNode();
        name = name + "/" + node->GetName();

        if (name.find("/P0D_") != std::string::npos
            && name.find("/X_") != std::string::npos
            && name.find("/Bar_") != std::string::npos) {
            // we found a P0D X bar.
            p0dXBarNodes.push_back(gGeoManager->GetCurrentNodeId());
            gGeoManager->CdUp();
            return;
        }

        if (name.find("/P0D_") != std::string::npos
            && name.find("/Y_") != std::string::npos
            && name.find("/Bar_") != std::string::npos) {
            // we found a P0D Y bar.
            p0dYBarNodes.push_back(gGeoManager->GetCurrentNodeId());
            gGeoManager->CdUp();
            return;
        }

        // Now do any daughters.
        for (int i=0; i<node->GetNdaughters(); ++i) {
            gGeoManager->CdDown(i);
            RecurseGeometry(depth+1,name);
        }

        gGeoManager->CdUp();
    }
}
namespace tut {
    struct baseP0DGeom {
        baseP0DGeom() {
            // Run before each test.
            GenerateND280MCEvents("P0DGeom");
        }
        ~baseP0DGeom() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseP0DGeom>::object testP0DGeom;
    test_group<baseP0DGeom> groupP0DGeom("P0DGeom");

    // Make sure there are events, and that P0D bars were found.
    template<> template<>
    void testP0DGeom::test<1> () {
        ensure("ND280 MC Events exist", detsim::Events.size()>0);
        ensure("TGeoManager is initialized", gGeoManager);
        gGeoManager->CdTop();
        detsim::RecurseGeometry(0,"");
        int p0dX = detsim::p0dXBarNodes.size();
        int p0dY = detsim::p0dYBarNodes.size();
        ensure_equals("Number of P0D Bars", p0dX+p0dY, 40*260);
        ensure_equals("Number of P0D X Bars", p0dX, 40*126);
        ensure_equals("Number of P0D Y Bars", p0dY, 40*134);
    }

    // Test to make sure that the positive Z for the X bars is at a positive Y
    // coordinate.
    template<> template<>
    void testP0DGeom::test<2> () {
        for (std::vector<int>::iterator bar = detsim::p0dXBarNodes.begin();
             bar != detsim::p0dXBarNodes.end();
             ++bar) {
            gGeoManager->CdNode(*bar);
            const TGeoBBox *barShape 
                = dynamic_cast<const TGeoBBox*>(gGeoManager->GetCurrentNode()
                                                ->GetVolume()->GetShape());
            ensure("Bar bounding box found",barShape);
            ensure_greaterthan("Bar length is more than 2 meters",
                               barShape->GetDZ(), 100*unit::cm);
            ensure_greaterthan("Bar width is more than 30 mm",
                               barShape->GetDX(), 15*unit::mm);
            ensure_greaterthan("Bar height is more than 16 mm",
                               barShape->GetDY(), 8*unit::mm);
            double loc[3] = {0.0, 0.0, barShape->GetDZ()};
            double mas[3];
            gGeoManager->LocalToMaster(loc,mas);
            ensure_greaterthan("Local Z in X bar is a positive Y",mas[1],0.0);
        }
    }

    // Test to make sure that the positive Z for the Y bars is at a positive X
    // coordinate.
    template<> template<>
    void testP0DGeom::test<3> () {
        for (std::vector<int>::iterator bar = detsim::p0dYBarNodes.begin();
             bar != detsim::p0dYBarNodes.end();
             ++bar) {
            gGeoManager->CdNode(*bar);
            const TGeoBBox *barShape 
                = dynamic_cast<const TGeoBBox*>(gGeoManager->GetCurrentNode()
                                                ->GetVolume()->GetShape());
            ensure("Bar bounding box found",barShape);
            ensure_greaterthan("Bar length is more than 2 meters",
                               barShape->GetDZ(), 100*unit::cm);
            ensure_greaterthan("Bar width is more than 30 mm",
                               barShape->GetDX(), 15*unit::mm);
            ensure_greaterthan("Bar height is more than 16 mm",
                               barShape->GetDY(), 8*unit::mm);
            double loc[3] = {0.0, 0.0, barShape->GetDZ()};
            double mas[3];
            gGeoManager->LocalToMaster(loc,mas);
            ensure_greaterthan("Local Z in Y bar is a positive X",
                               mas[0], 100*unit::cm);
        }
    }

    // Test to make sure the bars material is ScintillatorAvg.
    template<> template<>
    void testP0DGeom::test<4> () {
        for (std::vector<int>::iterator bar = detsim::p0dXBarNodes.begin();
             bar != detsim::p0dXBarNodes.end();
             ++bar) {
            gGeoManager->CdNode(*bar);
            const TGeoNode* node 
                = dynamic_cast<const TGeoNode*>(gGeoManager->GetCurrentNode());
            ensure("Valid node", node);
            const TGeoVolume* vol 
                = dynamic_cast<const TGeoVolume*>(node->GetVolume());
            ensure("Valid volume", vol);
            const TGeoMedium* med 
                = dynamic_cast<const TGeoMedium*>(vol->GetMedium());
            ensure("Valid medium",med);
            std::string mediumName = med->GetName();
            if (mediumName.find("ScintillatorAvg") == std::string::npos) {
                ND280Error("Bar material is " << mediumName  
                           << " but expected ScintillatorAvg");
            }
            ensure("Bar material is ScintillatorAvg",
                   mediumName.find("ScintillatorAvg") != std::string::npos);
            ensure_equals("Scintillator bar does not have daughters",
                          vol->GetNdaughters(), 0);
        }
    }

};
