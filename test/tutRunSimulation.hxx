#ifndef tutRunSimulation_hxx_seen
#include <vector>
#include <string> 

#include <TEvent.hxx>

namespace detsim {
    typedef std::vector<CP::TEvent*> EventVector;
    extern EventVector Events;
};

void GenerateND280MCEvents(std::string caller);
#endif
