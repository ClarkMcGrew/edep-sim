#ifndef EDepSim_VConstrainedPositionGenerator_hh_seen
#define EDepSim_VConstrainedPositionGenerator_hh_seen

#include <vector>

#include <G4ThreeVector.hh>

#include "kinem/EDepSimVPositionGenerator.hh"

/// Select a position and time to be used as the vertex of a primary particle,
/// but which is constrained by various standard tests.
namespace EDepSim {class VConstrainedPositionGenerator;}
class EDepSim::VConstrainedPositionGenerator : public EDepSim::VPositionGenerator {
public:
    VConstrainedPositionGenerator(const G4String& name);
    virtual ~VConstrainedPositionGenerator();

    /// Flag if the vertex should be forced to the candidate vertex returned
    /// by GetPosition().
    virtual bool ForcePosition();

    class PositionTest {
    public:
        PositionTest() {}
        virtual ~PositionTest() {}
        virtual bool Apply(const G4LorentzVector& /* vtx */) {return true;}
    };
    typedef std::vector<PositionTest*> PositionTests;

    /// Set the name of the volume to be sampled for a vertex.
    void SetVolumeName(const G4String& volume) {fSampleVolume = volume;}

    /// Clear the current set of vertex checks.
    void ClearPositionTests(void) {fPositionTests.clear();}

    /// Check that the vertex is inside of a volume specified by name.  Name
    /// may be a sub-string contanied in the full volume name.
    void CheckVolumeName(const G4String& name);

    /// Check that the vertex is not inside of a volume specified by name.
    /// Name may be a sub-string contained in the full volume name.
    void CheckNotVolumeName(const G4String& name);

    /// Check that the vertex is inside of a material specified by name.
    void CheckVolumeMaterial(const G4String& name);

    /// Check that the vertex is not inside of a material specified by name.
    void CheckNotVolumeMaterial(const G4String& name);

    /// Check that the vertex X position is greater than some value.
    void CheckMinX(double x);
    
    /// Check that the vertex X position is less than some value.
    void CheckMaxX(double x);
    
    /// Check that the vertex Y position is greater than some value.
    void CheckMinY(double y);
    
    /// Check that the vertex Y position is less than some value.
    void CheckMaxY(double y);
    
    /// Check that the vertex Z position is greater than some value.
    void CheckMinZ(double z);
    
    /// Check that the vertex Z position is less than some value.
    void CheckMaxZ(double z);

protected: 
    /// Return true if the vertex is valid.  This is used in the derived class.
    virtual bool ValidPosition(const G4LorentzVector& vtx);

    /// Generate a trial position uniformly in the sample box.
    G4LorentzVector TrialPosition();

private:
    /// The name of the volume to be sampled
    G4String fSampleVolume;

    /// True if the limits have been initialized.
    bool fLimitsFound;

    /// Find the limits of the volume to be sampled for a good vertex.
    void FindLimits();

    /// The vertex tests to apply.
    PositionTests fPositionTests;

    /// The lower boundary of the volume to be sampled for the vertex.
    G4ThreeVector fMinimumCorner;

    /// The upper boundary of the volume to be sampled for the vertex.
    G4ThreeVector fMaximumCorner;
};
#endif
