#ifndef EDepSim_FixedCountGenerator_hh_seen
#define EDepSim_FixedCountGenerator_hh_seen

#include "kinem/EDepSimVCountGenerator.hh"

namespace EDepSim {class FixedCountGenerator;}
class EDepSim::FixedCountGenerator : public EDepSim::VCountGenerator {
public:
    FixedCountGenerator(G4String name, int count, double intensity);
    virtual ~FixedCountGenerator();

    /// Return the number of events to generate.
    int GetCount();

private:
    /// The number of events to generate.
    int fCount;
};
#endif
