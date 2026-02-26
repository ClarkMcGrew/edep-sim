#ifndef EDepSimDokeBirksSaturation_h
#define EDepSimDokeBirksSaturation_h 1

#include <globals.hh>
#include <G4EmSaturation.hh>

class G4ParticleDefinition;
class G4MaterialCutsCouple;

/// Implement the Doke-Birk recombination probability found in NEST in an
/// optimized form for a LAr.  The Doke-Birk parameterization is quite
/// accurate for LAr over the important energy ranges for a LArTPC (E more
/// than about an MeV).
///
/// When this is called for a material other than LAr, then the default G4
/// Birk's Law implementation (G4EmSaturation) is used.
///
/// This file should always be accompanied by the full NEST implementation
/// since this is a direct modification of the NEST code.  The NEST code needs
/// to be provided since it is the arbitor of the performance of this code.
/// Where this code deviates from NEST, NEST is right.  Finally, if this is
/// used, the NEST authors should be cited.  They did all of the physics.
/// This is just an adaptation of their work to LAr so it's faster, but with
/// much less capability.
namespace EDepSim {class DokeBirksSaturation;}
class EDepSim::DokeBirksSaturation: public G4EmSaturation
{
public:

    explicit DokeBirksSaturation(G4int verb);
    virtual ~DokeBirksSaturation();

    /// Override the base VisibleEnergyDeposition with one that is aware of
    /// the Doke Birks model for LAr ionization saturation.
    virtual G4double VisibleEnergyDeposition(const G4ParticleDefinition*,
                                             const G4MaterialCutsCouple*,
                                             G4double length,
                                             G4double edepTotal,
                                             G4double edepNIEL = 0.0) const;

    // hide assignment operator
    DokeBirksSaturation & operator=(
        const DokeBirksSaturation &right) = delete;
    DokeBirksSaturation(const DokeBirksSaturation&) = delete;

private:

    /// Calculate the electron "Linear Energy Transfer". This is the dE/Dx
    /// without the denisity correction, so dEdX/density.  This is only
    /// applicable to LAr.
    G4double CalculateElectronLET ( G4double E) const;

};
#endif
