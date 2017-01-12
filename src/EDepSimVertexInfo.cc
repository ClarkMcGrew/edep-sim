#include <G4PrimaryVertex.hh>

#include "EDepSimVertexInfo.hh"

EDepSim::VertexInfo::VertexInfo()
    : fInformationalVertex(NULL), 
      fInteractionNumber(0), fCrossSection(0.0), fDiffCrossSection(0.0),
      fWeight(1.0), fProbability(1.0) {}

EDepSim::VertexInfo::~VertexInfo() {
    if (fInformationalVertex != NULL) delete fInformationalVertex;
}

const G4PrimaryVertex* EDepSim::VertexInfo::GetInformationalVertex(G4int i) const {
    if (i == 0) {
        return fInformationalVertex;
    }
    if (i<0) return NULL;
    const G4PrimaryVertex* infoVertex = fInformationalVertex;
    for (int j=0; j < i; j++) {
        if (infoVertex == NULL) return NULL;
        infoVertex = infoVertex->GetNext();
    }
    return infoVertex;
}

void EDepSim::VertexInfo::AddInformationalVertex(G4PrimaryVertex* vtx) {
    if (!fInformationalVertex) {
        fInformationalVertex = vtx; 
    }
    else { 
        fInformationalVertex->SetNext(vtx);
    }
}

int EDepSim::VertexInfo::GetNumberOfInformationalVertex() const {
    int number = 0;
    const G4PrimaryVertex* infoVertex = fInformationalVertex;
    for (int j=0; j < 1000; j++) {
        if (infoVertex == NULL) return number;
        ++number;
        infoVertex = infoVertex->GetNext();
    }
    return number;
}
