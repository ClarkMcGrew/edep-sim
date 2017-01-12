#ifndef EDepSim_UniformPositionGenerator_hh_seen
#define EDepSim_UniformPositionGenerator_hh_seen

#include "kinem/EDepSimVConstrainedPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
namespace EDepSim {class UniformPositionGenerator;}
class EDepSim::UniformPositionGenerator : public EDepSim::VConstrainedPositionGenerator {
public:
    UniformPositionGenerator(const G4String& name);
    virtual ~UniformPositionGenerator();

    /// Return a candidate vertex.
    virtual G4LorentzVector GetPosition();

    /// Flag if the vertex should be forced to the candidate vertex returned
    /// by GetPosition().
    virtual bool ForcePosition();
};
#endif
