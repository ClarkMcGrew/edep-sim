#ifndef EDepSim_PhysicsList_h
#define EDepSim_PhysicsList_h 1

#include "globals.hh"
#include "G4VModularPhysicsList.hh"

class G4VPhysicsConstructor;
namespace EDepSim {class PhysicsListMessenger;}
namespace EDepSim {class ExtraPhysics;}

/// Use the G4PhysListFactory to select a physics list for this run.  The
/// physics list can be set from the PHYSLIST environment variable.
namespace EDepSim {class PhysicsList;}
class EDepSim::PhysicsList: public G4VModularPhysicsList {
public:

    /// Construct the physics list.  If the PHYSLIST environment variable is
    /// set, then this will use the GEANT4 physics list factory to construct
    /// the named physics list.  Otherwise, this will check if physName is a
    /// known physics list name to construct a physics list.  If neither of
    /// those options are successful, the default list will be constructed
    /// (originally QSGP_BERT).  If all of thoses methods fail, then exit with
    /// an exception.
    ///
    /// Note: The string value for physName is expected to be provided as an
    /// edep-sim command line argument. This is usually called before the UI
    /// macro processor is ready.
    explicit PhysicsList(G4String physName);

    virtual ~PhysicsList();

    /// Used by GEANT4 to set the cuts defined below.
    virtual void SetCuts();

    /// Set the range cut for photons.
    void SetCutForGamma(G4double);

    /// Set the range cut for electrons.
    void SetCutForElectron(G4double);

    /// Set the range cut for positrons.
    void SetCutForPositron(G4double);

    /// Set the recombination fraction for liquid argon (negative for using
    /// nest).
    void SetIonizationModel(bool);

private:

    /// The gamma-ray range cut.
    G4double fCutForGamma;

    /// The electron range cut.
    G4double fCutForElectron;

    /// The positron range cut.
    G4double fCutForPositron;

    /// The extra physics list
    EDepSim::ExtraPhysics* fExtra;

    /// The messenger to control this class.
    EDepSim::PhysicsListMessenger* fMessenger;

};

#endif
