#ifndef EDepSim_FreeTimeGenerator_hh_seen
#define EDepSim_FreeTimeGenerator_hh_seen

#include "kinem/EDepSimVTimeGenerator.hh"

namespace EDepSim {class FreeTimeGenerator;}

/// Generate a "Free" time.  This means that the default time of the vertex
/// will be zero, but it is expected to be overridden by the kinematics
/// generator.
class EDepSim::FreeTimeGenerator : public EDepSim::VTimeGenerator {
public:
    FreeTimeGenerator(G4String name);
    virtual ~FreeTimeGenerator();

    /// Return the time of events to generate.
    double GetTime(const G4LorentzVector& vertex);

    /// Flag if the time should be forced (returns false).
    virtual bool ForceTime();

};
#endif
