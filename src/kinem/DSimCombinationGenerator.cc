// There isn't any standard interface to remove particles from the vertex,
// or vertices from the event.  This has to grunge around in the G4
// internals.  This depends on an evil hack.
#if __GNUC__ < 5
#define private public
#include <G4PrimaryParticle.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#undef private
#endif

#include "DSimLog.hh"
#include "DSimException.hh"
#include "DSimCombinationGenerator.hh"

DSimCombinationGenerator::DSimCombinationGenerator() {}

DSimCombinationGenerator::~DSimCombinationGenerator(void) {}

void DSimCombinationGenerator::GeneratePrimaryVertex(G4Event* evt) {
#if __GNUC__ > 4
    DSimThrow("DSimCombinationGenerator is not supported");
#else
    DSimInfo("# Combine previous two vertices");

    // Find the second to last vertex.
    G4PrimaryVertex* vtx = NULL;
    G4PrimaryVertex* lastVtx = evt->GetPrimaryVertex();
    for (;lastVtx && lastVtx->GetNext();lastVtx = lastVtx->GetNext()) {
        vtx = lastVtx;
    }
    if (!vtx) return;
    if (!lastVtx) return;

    // Get the particles from the last vertex.
    std::vector<G4PrimaryParticle*> particles;
    for (int i = 0; i<lastVtx->GetNumberOfParticle(); ++i) {
        G4PrimaryParticle* particle = lastVtx->GetPrimary(i);
        if (particle) particles.push_back(particle);
    }

    /// Reset the internal parameters.
    for (std::vector<G4PrimaryParticle*>::iterator p = particles.begin();
         p != particles.end();
         ++p) {
        (*p)->nextParticle = NULL;
    }

    // Add the particles to the previous vertex.
    for (std::vector<G4PrimaryParticle*>::iterator p = particles.begin();
         p != particles.end();
         ++p) {
        vtx->SetPrimary((*p));
    }

    // Delete the last vertex.
    lastVtx->theParticle = NULL;
    lastVtx->theTail = NULL;
    vtx->nextVertex = lastVtx->nextVertex;
    vtx->tailVertex = lastVtx->tailVertex;
    lastVtx->nextVertex = NULL;
    lastVtx->tailVertex = NULL;
    delete lastVtx;
    --evt->numberOfPrimaryVertex;
#endif
}

G4String DSimCombinationGenerator::GetName() {
    return "combination";
}
