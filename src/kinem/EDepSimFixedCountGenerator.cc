#include "kinem/EDepSimVCountGenerator.hh"
#include "kinem/EDepSimFixedCountGenerator.hh"

EDepSim::FixedCountGenerator::FixedCountGenerator(G4String name,
                                                   int count,
                                                   double intensity) 
    : EDepSim::VCountGenerator(name,intensity), fCount(count) {}

EDepSim::FixedCountGenerator::~FixedCountGenerator() {}

int EDepSim::FixedCountGenerator::GetCount() {return fCount;}
