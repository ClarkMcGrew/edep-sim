#ifndef DSimGPSKinematicsGenerator_hh_Seen
#define DSimGPSKinematicsGenerator_hh_Seen

#include <globals.hh>
#include <G4LorentzVector.hh>

#include "kinem/DSimVKinematicsGenerator.hh"

class G4Event;
class G4VPrimaryGenerator;

/// A base class for primary generator classes used in the DSim.  This is
/// used by the DSimUserPrimaryGeneratorAction to generate particles which
/// will be tracked by the G4 simulation.
class DSimGPSKinematicsGenerator : public DSimVKinematicsGenerator {
public:
    DSimGPSKinematicsGenerator(const G4String& name, 
                                G4VPrimaryGenerator* gps);
    virtual ~DSimGPSKinematicsGenerator();

    /// Add a primary vertex to the event.  
    virtual bool GeneratePrimaryVertex(G4Event* evt,
                                       G4LorentzVector& position);

    /// Return the mean event rate per mass per 10^21 pot at a particular
    /// position and in a particular material.
    virtual double EventRate(const G4ThreeVector& pos,
                             const G4Material& material) {
        return 1.0/kg;
    }

private:
    G4VPrimaryGenerator* fGenerator;
};
#endif
