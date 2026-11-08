#ifndef EDepSim_PrimaryVertexGenerator_hh
#define EDepSim_PrimaryVertexGenerator_hh 1

// EDepSimPrimaryVertexGenerator.hh
//
// A G4VPrimaryGenerator driven by an in-memory TG4PrimaryVertexContainer.  It
// is a *direct* G4VPrimaryGenerator (not an EDepSim::PrimaryGenerator
// composite): the container already carries vertex positions and times, so the
// position/time/count sub-generators are bypassed entirely.  Values in the
// container are in Geant4/CLHEP units (see io/EDepSimUnits.h), which are exactly
// the units G4PrimaryVertex/G4PrimaryParticle expect, so no scaling is applied.

#include "TG4PrimaryVertex.h"   // TG4PrimaryVertexContainer

#include <G4VPrimaryGenerator.hh>

#include <memory>

class G4Event;

namespace EDepSim {

    class PrimaryVertexGenerator : public G4VPrimaryGenerator {
    public:
        PrimaryVertexGenerator();
        ~PrimaryVertexGenerator() override;

        /// Set the primaries to be built by the next GeneratePrimaryVertex.
        void Feed(std::shared_ptr<const TG4PrimaryVertexContainer> primaries);

        /// G4VPrimaryGenerator: build G4 primaries for one event from the most
        /// recently fed container.
        void GeneratePrimaryVertex(G4Event* event) override;

    private:
        std::shared_ptr<const TG4PrimaryVertexContainer> fPrimaries;
    };

} // namespace EDepSim
#endif
