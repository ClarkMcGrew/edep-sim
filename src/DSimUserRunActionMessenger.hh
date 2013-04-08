////////////////////////////////////////////////////////////
// $Id: DSimUserRunActionMessenger.hh,v 1.4 2011/07/19 20:57:44 mcgrew Exp $
//
#ifndef DSimUserRunActionMessenger_h
#define DSimUserRunActionMessenger_h 1

#include <map>
#include "G4UImessenger.hh"
#include "globals.hh"

class DSimUserRunAction;

class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class DSimUserRunActionMessenger: public G4UImessenger {
public:
    DSimUserRunActionMessenger(DSimUserRunAction*);
    virtual ~DSimUserRunActionMessenger();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

    /// Get the base directory for the messenger commands.
    G4String GetPath();

private:
    // need primary generator action to change the seed
    DSimUserRunAction*  fUserRunAction; 
    
    G4UIdirectory* fDir;
    G4UIcmdWithAnInteger* fRandomSeedCmd;
    G4UIcmdWithoutParameter* fTimeRandomSeedCmd;
    G4UIcmdWithoutParameter* fShowRandomSeedCmd;
    G4UIcmdWithAnInteger* fDetSimRunIdCmd;
    G4UIcmdWithAnInteger* fDetSimSubrunIdCmd;

};
#endif
