#ifndef EDepSim_FixedTimeGenerator_hh_seen
#define EDepSim_FixedTimeGenerator_hh_seen

#include "kinem/EDepSimVTimeGenerator.hh"

namespace EDepSim {class FixedTimeGenerator;}
class EDepSim::FixedTimeGenerator : public EDepSim::VTimeGenerator {
public:
    FixedTimeGenerator(G4String name, double time);
    virtual ~FixedTimeGenerator();

    /// Return the number of events to generate.
    double GetTime(const G4LorentzVector& vertex);

    /// Flag if the time should be forced.
    virtual bool ForceTime();

private:
    /// The number of events to generate.
    double fTime;
};
#endif
