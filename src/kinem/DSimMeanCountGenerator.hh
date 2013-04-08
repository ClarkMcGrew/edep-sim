#ifndef DSimMeanCountGenerator_hh_seen
#define DSimMeanCountGenerator_hh_seen

#include "kinem/DSimVCountGenerator.hh"

class DSimMeanCountGenerator : public DSimVCountGenerator {
public:
    DSimMeanCountGenerator(G4String name, double mean, double intensity);
    virtual ~DSimMeanCountGenerator();

    /// Return the number of events to generate.
    int GetCount();

private:
    /// The number of events to generate.
    double fMean;
};
#endif
