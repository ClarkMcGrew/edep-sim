#ifndef EDepSim_ExternalKinematicsFactory_hh_seen
#define EDepSim_ExternalKinematicsFactory_hh_seen

#include "kinem/EDepSimVKinematicsFactory.hh"

#include <G4UImessenger.hh>

class G4VPrimaryGenerator;
namespace EDepSim {class ExternalKinematicsGenerator;}

namespace EDepSim {class ExternalKinematicsFactory;}
class EDepSim::ExternalKinematicsFactory : public EDepSim::VKinematicsFactory {
public:
    ExternalKinematicsFactory(
        EDepSim::UserPrimaryGeneratorMessenger* fParent);
    virtual ~ExternalKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual EDepSim::VKinematicsGenerator* GetGenerator();

    /// Set the name of the shared library to load.
    void SetLibraryPath(const G4String& s) {fLibraryPath = s;}

    /// Set the name of the shared library to load.
    void SetSymbolName(const G4String& s) {fSymbolName = s;}

    /// Set the option passed to the external constructor.
    void SetOption(const G4String& n) {fUserOption = n;}

    /// Handle the macro command inputs.
    virtual void SetNewValue(G4UIcommand* command,G4String newValue);

private:

    /// The name of the shared library file.
    G4String fLibraryPath;

    /// The name of the symbol in the library.
    G4String fSymbolName;

    /// The name of the generator passed to the external symbol
    G4String fUserOption;

    /// A command connect to a shared library.
    G4UIcommand* fExternalKinemCMD;
    G4UIcmdWithAString* fLibraryPathCMD;
    G4UIcmdWithAString* fSymbolNameCMD;
    G4UIcmdWithAString* fUserOptionCMD;

};
#endif
