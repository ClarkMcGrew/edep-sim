#ifndef EDepSim_FreePositionGenerator_hh_seen
#define EDepSim_FreePositionGenerator_hh_seen

#include "kinem/EDepSimVPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
namespace EDepSim {class FreePositionGenerator;}
class EDepSim::FreePositionGenerator : public EDepSim::VPositionGenerator {
public:
    FreePositionGenerator(const G4String& name);
    virtual ~FreePositionGenerator();

    /// Return a candidate vertex (a dummy routine for
    /// EDepSim::FreePositionGenerator).
    virtual G4LorentzVector GetPosition();

    /// Flag if the vertex should be forced to the candidate vertex returned
    /// by GetPosition().
    virtual bool ForcePosition();
};
#endif
