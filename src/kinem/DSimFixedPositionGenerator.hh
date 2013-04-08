#ifndef DSimFixedPositionGenerator_hh_seen
#define DSimFixedPositionGenerator_hh_seen

#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>

#include "kinem/DSimVPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
class DSimFixedPositionGenerator : public DSimVPositionGenerator {
public:
    DSimFixedPositionGenerator(const G4String& name, 
                                const G4ThreeVector& pos);
    virtual ~DSimFixedPositionGenerator();

    /// Return a candidate vertex.
    virtual G4LorentzVector GetPosition();

    /// Set the position for the vertex.
    virtual void SetPosition(const G4ThreeVector& pos);

    /// Flag if the vertex should be forced to the candidate vertex returned
    /// by GetPosition().
    virtual bool ForcePosition();

private:
    /// The position for this generator.
    G4LorentzVector fPosition;
};
#endif
