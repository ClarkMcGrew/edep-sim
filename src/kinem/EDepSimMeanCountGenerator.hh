#ifndef EDepSim_MeanCountGenerator_hh_seen
#define EDepSim_MeanCountGenerator_hh_seen

#include "kinem/EDepSimVCountGenerator.hh"

namespace EDepSim {class MeanCountGenerator;}
class EDepSim::MeanCountGenerator : public EDepSim::VCountGenerator {
public:
    MeanCountGenerator(G4String name, double mean, double intensity);
    virtual ~MeanCountGenerator();

    /// Return the number of events to generate.
    int GetCount();

private:
    /// The number of events to generate.
    double fMean;
};
#endif
