#include <iostream>
#include <tut.h>

#include "tutRunSimulation.hxx"

namespace tut {
    struct baseEvent {
        baseEvent() {
            // Run before each test.
            GenerateND280MCEvents("Event");
        }
        ~baseEvent() {
            // Run after each test.
        }
    };

    // Declare the test
    typedef test_group<baseEvent>::object testEvent;
    test_group<baseEvent> groupEvent("Event");

    // Test the default constructor and destructor.  Make sure the default
    // values are all null.
    template<> template<>
    void testEvent::test<1> () {
        ensure("ND280 MC Events exist", detsim::Events.size()>0);
    }

};
