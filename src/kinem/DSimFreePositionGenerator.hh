#ifndef DSimFreePositionGenerator_hh_seen
#define DSimFreePositionGenerator_hh_seen

#include "kinem/DSimVPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
class DSimFreePositionGenerator : public DSimVPositionGenerator {
public:
    DSimFreePositionGenerator(const G4String& name);
    virtual ~DSimFreePositionGenerator();

    /// Return a candidate vertex (a dummy routine for
    /// DSimFreePositionGenerator).
    virtual G4LorentzVector GetPosition();

    /// Flag if the vertex should be forced to the candidate vertex returned
    /// by GetPosition().
    virtual bool ForcePosition();
};
#endif
