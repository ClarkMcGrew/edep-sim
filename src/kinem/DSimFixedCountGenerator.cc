#include "kinem/DSimVCountGenerator.hh"
#include "kinem/DSimFixedCountGenerator.hh"

DSimFixedCountGenerator::DSimFixedCountGenerator(G4String name,
                                                   int count,
                                                   double intensity) 
    : DSimVCountGenerator(name,intensity), fCount(count) {}

DSimFixedCountGenerator::~DSimFixedCountGenerator() {}

int DSimFixedCountGenerator::GetCount() {return fCount;}
