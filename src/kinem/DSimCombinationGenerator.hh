#ifndef DSimCombinationGenerator_hh_Seen
#define DSimCombinationGenerator_hh_Seen

class G4Event;
#include <G4VPrimaryGenerator.hh>

/// Copy the vertex position from the source to the destination
/// G4PrimaryVertex object.  This has the effect of making the particles in
/// the vertices come from the same location (or vertex).  This also has an
/// option to set the position of the destination vertex relative to the
/// source vertex.  When this option is used, the source vertex position is
/// added to the destination vertex (i.e. if the source vertex is [10,10,10]
/// and the destination vertex is originally [1,1,1] the destination vertex
/// becomes [11,11,11].
class DSimCombinationGenerator: public G4VPrimaryGenerator {
public:
    DSimCombinationGenerator(int src, int dest, bool relative);
    virtual ~DSimCombinationGenerator();

    /// A pure virtual method to generate the actual primary particles which
    /// must be implemented in each derived class.
    virtual void GeneratePrimaryVertex(G4Event* evt);
    
    /// Return the name of this generator.
    G4String GetName();
};
#endif
