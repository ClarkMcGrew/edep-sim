#ifndef EDepSim_TrackingTestPrimaries_hh
#define EDepSim_TrackingTestPrimaries_hh 1

// Shared helper for the TrackingService integration tests: build a
// TG4PrimaryVertexContainer holding one vertex with a single forward muon.
// Values are in Geant4/CLHEP units (mm, MeV).
//
// A producer that WRITES the i/o fields must be COMPILED with
// -DEDEPSIM_USE_PUBLIC_FIELDS: TG4PrimaryParticle's fields are otherwise private
// (they are parsed before TG4PrimaryVertex.h defines that macro mid-header, and
// its include guard prevents a later re-parse).  So the define has to be on the
// command line / target_compile_definitions, not #defined here.
#include "TG4PrimaryVertex.h"

#include <cmath>
#include <memory>

namespace edepsim_ctest {

    inline std::shared_ptr<const TG4PrimaryVertexContainer>
    one_muon(double p_MeV = 1000.0) {
        auto primaries = std::make_shared<TG4PrimaryVertexContainer>();

        TG4PrimaryVertex vtx;
        vtx.Position.SetXYZT(0.0, 0.0, 0.0, 0.0);   // origin, inside the world

        TG4PrimaryParticle mu;
        mu.PDGCode = 13;                            // mu-
        const double mass = 105.6583745;            // MeV
        const double E = std::sqrt(p_MeV * p_MeV + mass * mass);
        mu.Momentum.SetPxPyPzE(0.0, 0.0, p_MeV, E); // forward along +z
        vtx.Particles.push_back(mu);

        primaries->push_back(vtx);
        return primaries;
    }

} // namespace edepsim_ctest
#endif
