#include <G4Poisson.hh>

#include "kinem/EDepSimVCountGenerator.hh"
#include "kinem/EDepSimMeanCountGenerator.hh"

EDepSim::MeanCountGenerator::MeanCountGenerator(G4String name,
                                                 double mean,
                                                 double intensity) 
    : EDepSim::VCountGenerator(name, intensity), fMean(mean) {}

EDepSim::MeanCountGenerator::~MeanCountGenerator() {}

int EDepSim::MeanCountGenerator::GetCount() {
    return G4Poisson(fMean);
}
