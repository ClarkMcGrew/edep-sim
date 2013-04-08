#ifndef DSimUniformPositionGenerator_hh_seen
#define DSimUniformPositionGenerator_hh_seen

#include "kinem/DSimVConstrainedPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
class DSimUniformPositionGenerator : public DSimVConstrainedPositionGenerator {
public:
    DSimUniformPositionGenerator(const G4String& name);
    virtual ~DSimUniformPositionGenerator();

    /// Return a candidate vertex.
    virtual G4LorentzVector GetPosition();

    /// Flag if the vertex should be forced to the candidate vertex returned
    /// by GetPosition().
    virtual bool ForcePosition();
};
#endif
