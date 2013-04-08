#ifndef DSimFixedCountGenerator_hh_seen
#define DSimFixedCountGenerator_hh_seen

#include "kinem/DSimVCountGenerator.hh"

class DSimFixedCountGenerator : public DSimVCountGenerator {
public:
    DSimFixedCountGenerator(G4String name, int count, double intensity);
    virtual ~DSimFixedCountGenerator();

    /// Return the number of events to generate.
    int GetCount();

private:
    /// The number of events to generate.
    int fCount;
};
#endif
