// EDepSimPrimaryVertexGenerator.cc

#include "EDepSimPrimaryVertexGenerator.hh"

#include <G4Event.hh>
#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>

#include <utility>

namespace EDepSim {

    PrimaryVertexGenerator::PrimaryVertexGenerator() = default;
    PrimaryVertexGenerator::~PrimaryVertexGenerator() = default;

    void PrimaryVertexGenerator::Feed(
        std::shared_ptr<const TG4PrimaryVertexContainer> primaries) {
        fPrimaries = std::move(primaries);
    }

    void PrimaryVertexGenerator::GeneratePrimaryVertex(G4Event* event) {
        if (!fPrimaries) return;

        for (const TG4PrimaryVertex& vtx : *fPrimaries) {
            // Position/time are already in Geant4/CLHEP units (mm, ns).
            const TLorentzVector& pos = vtx.GetPosition();
            G4PrimaryVertex* g4vtx =
                new G4PrimaryVertex(pos.X(), pos.Y(), pos.Z(), pos.T());

            for (const TG4PrimaryParticle& part : vtx.Particles) {
                // Momentum/energy are already in Geant4/CLHEP units (MeV).
                const TLorentzVector& mom = part.GetMomentum();
                g4vtx->SetPrimary(new G4PrimaryParticle(part.GetPDGCode(),
                                                        mom.Px(), mom.Py(),
                                                        mom.Pz(), mom.E()));
            }

            event->AddPrimaryVertex(g4vtx);
        }
    }

} // namespace EDepSim
