#ifndef DSimCombinationGenerator_hh_Seen
#define DSimCombinationGenerator_hh_Seen

class G4Event;
#include <G4VPrimaryGenerator.hh>

/// Combine the two most recent G4PrimaryVertex objects into a single vertex.
/// The particles from the most recent G4PrimaryVertex (last on the list) are
/// added to the previous G4PrimaryVertex using the time and position from the
/// previous vertex.  If there are fewer than two primary vertices, then this
/// doesn't do anything.
class DSimCombinationGenerator: public G4VPrimaryGenerator {
public:
    DSimCombinationGenerator();
    virtual ~DSimCombinationGenerator();

    /// A pure virtual method to generate the actual primary particles which
    /// must be implemented in each derived class.
    virtual void GeneratePrimaryVertex(G4Event* evt);
    
    /// Return the name of this generator.
    G4String GetName();
};
#endif
