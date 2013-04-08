#include <iostream>
#include <vector>
#include <string>

#include <tut.h>

#include <TGeoManager.h>
#include <TGeoOverlap.h>
#include <TGeoShape.h>
#include <TGeoBBox.h>

#include <HEPUnits.hxx>
#include <TManager.hxx>
#include <TEvent.hxx>

#include "tutRunSimulation.hxx"

namespace tut {
    struct baseCheckGeom {
        baseCheckGeom() {
            // Run before each test.
            GenerateND280MCEvents("CheckGeom");
        }
        ~baseCheckGeom() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseCheckGeom>::object testCheckGeom;
    test_group<baseCheckGeom> groupCheckGeom("CheckGeom");

    // Make sure there are events.
    template<> template<>
    void testCheckGeom::test<1> () {
        ensure("ND280 MC Events exist", detsim::Events.size()>0);
        ensure("TGeoManager is initialized", gGeoManager);
        gGeoManager->CdTop();
    }

    // Test to make sure that the positive Z for the X bars is at a positive Y
    // coordinate.
    template<> template<>
    void testCheckGeom::test<2> () {
        gGeoManager->CheckOverlaps();
        TIter next(gGeoManager->GetListOfOverlaps());
        TGeoOverlap *overlap;
        int count = 0;
        while ((overlap = (TGeoOverlap*)next())) {
            if (count<50) overlap->PrintInfo();
            ++count;
        }
        ensure_lessthan("No geometry overlaps found", count, 1);
    }
};
