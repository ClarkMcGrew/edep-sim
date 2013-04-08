#ifndef DSimFixedTimeGenerator_hh_seen
#define DSimFixedTimeGenerator_hh_seen

#include "kinem/DSimVTimeGenerator.hh"

class DSimFixedTimeGenerator : public DSimVTimeGenerator {
public:
    DSimFixedTimeGenerator(G4String name, double time);
    virtual ~DSimFixedTimeGenerator();

    /// Return the number of events to generate.
    double GetTime(const G4LorentzVector& vertex);

    /// Flag if the time should be forced.
    virtual bool ForceTime();

private:
    /// The number of events to generate.
    double fTime;
};
#endif
