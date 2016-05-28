#ifndef DSimNuMIRockKinematicsGenerator_hh
#define DSimNuMIRockKinematicsGenerator_hh
////////////////////////////////////////////////////////////
//

#include "kinem/DSimVKinematicsGenerator.hh"

class G4Event;

/// This is an interface approximate the rock muons entering CAPTAIN.
class DSimNuMIRockKinematicsGenerator : public DSimVKinematicsGenerator {
public:
    ///  Construct a new generator.  The name is the name of the generator
    ///  (e.g. GENIE, rock, &c).  The fluxName is the name of the neutrino
    ///  spectrum.
    DSimNuMIRockKinematicsGenerator(const G4String& name, 
                                    const G4String& fluxName);
    virtual ~DSimNuMIRockKinematicsGenerator();

    /// Add a primary vertex to the event.  
    virtual bool GeneratePrimaryVertex(G4Event* evt,
                                       G4LorentzVector& position);

    /// Return the mean event rate per mass per 10^21 pot at a particular
    /// position and in a particular material.  The first parameter is the
    /// position to calculate the event rate at, and the second is the
    /// material at that position. 
    virtual double EventRate(const G4ThreeVector&,
                             const G4Material&) {
        return 1.0/kg;
    }

private:
    /// The static part of the file name field.
    std::string fFluxName;

};
#endif
