#include <G4Poisson.hh>

#include "kinem/DSimVCountGenerator.hh"
#include "kinem/DSimMeanCountGenerator.hh"

DSimMeanCountGenerator::DSimMeanCountGenerator(G4String name,
                                                 double mean,
                                                 double intensity) 
    : DSimVCountGenerator(name, intensity), fMean(mean) {}

DSimMeanCountGenerator::~DSimMeanCountGenerator() {}

int DSimMeanCountGenerator::GetCount() {
    return G4Poisson(fMean);
}
