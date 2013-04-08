#ifndef DSimDensityPositionGenerator_hh_seen
#define DSimDensityPositionGenerator_hh_seen

#include "kinem/DSimVConstrainedPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
class DSimDensityPositionGenerator : public DSimVConstrainedPositionGenerator {
public:
    DSimDensityPositionGenerator(const G4String& name);
    virtual ~DSimDensityPositionGenerator();

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
