////////////////////////////////////////////////////////////
// $Id: EDepSim::UserRunActionMessenger.hh,v 1.4 2011/07/19 20:57:44 mcgrew Exp $
//
#ifndef EDepSim_UserRunActionMessenger_h
#define EDepSim_UserRunActionMessenger_h 1

#include <map>
#include "G4UImessenger.hh"
#include "globals.hh"

namespace EDepSim {class UserRunAction;}

class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

namespace EDepSim {class UserRunActionMessenger;}
class EDepSim::UserRunActionMessenger: public G4UImessenger {
public:
    UserRunActionMessenger(EDepSim::UserRunAction*);
    virtual ~UserRunActionMessenger();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

    /// Get the base directory for the messenger commands.
    G4String GetPath();

private:
    // need primary generator action to change the seed
    EDepSim::UserRunAction*  fUserRunAction; 
    
    G4UIdirectory* fDir;
    G4UIcmdWithAnInteger* fRandomSeedCmd;
    G4UIcmdWithoutParameter* fTimeRandomSeedCmd;
    G4UIcmdWithoutParameter* fShowRandomSeedCmd;
    G4UIcmdWithAnInteger* fDetSimRunIdCmd;
    G4UIcmdWithAnInteger* fDetSimSubrunIdCmd;

};
#endif
