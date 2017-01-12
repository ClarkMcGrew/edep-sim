#ifndef EDepSim_DensityPositionGenerator_hh_seen
#define EDepSim_DensityPositionGenerator_hh_seen

#include "kinem/EDepSimVConstrainedPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
namespace EDepSim {class DensityPositionGenerator;}
class EDepSim::DensityPositionGenerator : public EDepSim::VConstrainedPositionGenerator {
public:
    DensityPositionGenerator(const G4String& name);
    virtual ~DensityPositionGenerator();

    /// Return a candidate vertex.
    virtual G4LorentzVector GetPosition();

    /// Flag if the vertex should be forced to the candidate vertex returned
    /// by GetPosition().
    virtual bool ForcePosition();

private:
    /// The maximum density in the detector.
    double fMaximumDensity;
};
#endif
