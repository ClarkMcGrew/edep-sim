#ifndef DSimVPrimaryFactory_hh_seen
#define DSimVPrimaryFactory_hh_seen

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcommand.hh>
#include <G4UIparameter.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

#include "DSimUserPrimaryGeneratorMessenger.hh"

class DSimVPrimaryFactory : public G4UImessenger {
public:
    DSimVPrimaryFactory(G4String subdir, G4String name, 
                         DSimUserPrimaryGeneratorMessenger* parent,
                         bool makeDirectory);
    virtual ~DSimVPrimaryFactory();

    /// Return the full path for the factory.  This returns the full name of
    /// the DSimVPrimaryFactory.  It should be used as a prefix for any
    /// messenger commands.  The full path for the factory is constructed as
    /// "<parent>/<subdir>/<localname>/".
    G4String GetPath() const;

    /// Return the short name of this factory.  This is used to identify it in
    /// other messenger commands.  The full path for the factory is
    /// constructed as "<parent>/<subdir>/<name>/".
    G4String GetName() const;

    /// Return the subdirectory name for this factory.  The full path for the
    /// factory is constructed as "<parent>/<subdir>/<name>/".
    G4String GetSubDir() const;

    /// Handle messages from the UI processor.
    virtual void SetNewValue(G4UIcommand*, G4String) {};

protected:
    /// Build a command name with the directory prefix.
    G4String CommandName(G4String cmd) {
        G4String name = GetPath() + cmd;
        return name;
    };

private:
    /// The short name of this factory.
    G4String fName;

    /// The sub-directory holding this factory.
    G4String fSubDirName;

    /// The messenger that is holding this set of sub-commands.
    DSimUserPrimaryGeneratorMessenger* fParent;

    /// The sub-directory for the local commands.  The full name of the local
    /// subdirectory is "<parent>/<subdir>/<name>/"
    G4UIdirectory* fDir;
};
#endif
