#ifndef TG4PrimaryVertex_hxx_seen
#define TG4PrimaryVertex_hxx_seen

#include <string>
#include <vector>

#include <TLorentzVector.h>

class TG4PrimaryParticle;

/// A class to save a G4 primary vertex into a root output file without linking
/// to geant.
class TG4PrimaryVertex : public TObject {
public:
    TG4PrimaryVertex(void)
        : Position(0,0,0,0), GeneratorName("none"),
          InteractionNumber(0), CrossSection(0.0), DiffCrossSection(0.0),
          Weight(0.0), Probability(0.0) {}
    virtual ~TG4PrimaryVertex();

    /// The initial position of the particle.
    TLorentzVector Position;

    /// The name of the generator that created this vertex.
    std::string GeneratorName;

    /// The reaction that created this vertex.
    std::string Reaction;

    /// The name of the input file.
    std::string Filename;

    /// The index (or identifier) of the interaction in the kinematics file.
    Int_t InteractionNumber;

    /// The cross section for the reaction that created this vertex.
    Float_t CrossSection;

    /// The differential cross section for the kinematics of the reaction that
    /// created this vertex.
    Float_t DiffCrossSection;

    /// The weight of the interaction.  This will be set to one if the
    /// interaction is not reweighted.  If the vertex is oversampled, this
    /// will be less than one.
    Float_t Weight;

    /// The overall probability of the interaction that created this vertex.
    /// This includes the effect of the cross section, path length through the
    /// material, etc.  This should be one if it is not filled.
    Float_t Probability;

    /// The PrimaryVertex points for this PrimaryVertex.
    std::vector<TG4PrimaryParticle> Particles;

    /// The informational vertices associated with this vertex.
    // std::vector<TG4PrimaryVertex> Informational;

    ClassDef(TG4PrimaryVertex,1)
};

/// A class to save a G4 primary particle into a root output file without
/// linking to geant.
class TG4PrimaryParticle : public TObject {
public:
    TG4PrimaryParticle(void)
        : TrackId(-1), PDGCode(0), Momentum(0,0,0,0) {}
    virtual ~TG4PrimaryParticle();

    /// The Track Id of the matching trajectory.  Particles that are not
    /// tracked will have negative track id values.
    Int_t TrackId;

    /// The name of the particle.
    std::string Name;
    
    /// The PDG code of the particle.
    Int_t PDGCode;

    /// The initial momentum of the particle
    TLorentzVector Momentum;

    ClassDef(TG4PrimaryParticle,1);
};
#endif
