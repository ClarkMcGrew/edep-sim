#ifndef EDepSim_FixedPositionGenerator_hh_seen
#define EDepSim_FixedPositionGenerator_hh_seen

#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>

#include "kinem/EDepSimVPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle.
namespace EDepSim {class FixedPositionGenerator;}
class EDepSim::FixedPositionGenerator : public EDepSim::VPositionGenerator {
public:
    FixedPositionGenerator(const G4String& name, 
                                const G4ThreeVector& pos);
    virtual ~FixedPositionGenerator();

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
