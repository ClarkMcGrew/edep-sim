////////////////////////////////////////////////////////////
// $Id: EDepSim::UserPrimaryGeneratorMessenger.hh,v 1.9 2012/05/10 16:27:26 mcgrew Exp $
//
#ifndef EDepSim_UserPrimaryGeneratorMessenger_h
#define EDepSim_UserPrimaryGeneratorMessenger_h 1

#include <map>

#include "G4UImessenger.hh"
#include "globals.hh"

namespace EDepSim {class UserPrimaryGeneratorAction;}
namespace EDepSim {class VKinematicsFactory;}
namespace EDepSim {class VCountFactory;}
namespace EDepSim {class VPositionFactory;}
namespace EDepSim {class VTimeFactory;}
namespace EDepSim {class PrimaryGenerator;}

class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

namespace EDepSim {class UserPrimaryGeneratorMessenger;}
class EDepSim::UserPrimaryGeneratorMessenger: public G4UImessenger {
public:
    UserPrimaryGeneratorMessenger(EDepSim::UserPrimaryGeneratorAction*);
    virtual ~UserPrimaryGeneratorMessenger();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

    /// Get the base directory for the messenger commands.
    G4String GetPath();

    /// Add a new kinematics factory to the messenger.
    void AddKinematicsFactory(EDepSim::VKinematicsFactory* factory);
    
    /// Set the current kinematics factory for the messenger.
    void SetKinematicsFactory(const G4String& factory);
    
    /// Get the list of kinematics factories available to the messenger.
    G4String GetKinematicsFactories();
    
    /// Add a new count factory to the messenger.
    void AddCountFactory(EDepSim::VCountFactory* factory);
    
    /// Set the current count factory for the messenger.
    void SetCountFactory(const G4String& factory);
    
    /// Get the list of count factories available to the messenger.
    G4String GetCountFactories();
    
    /// Add a new position factory to the messenger.
    void AddPositionFactory(EDepSim::VPositionFactory* factory);
    
    /// Set the current position factory for the messenger.
    void SetPositionFactory(const G4String& factory);
    
    /// Get the list of position factories available to the messenger.
    G4String GetPositionFactories();

    /// Add a new time factory to the messenger.
    void AddTimeFactory(EDepSim::VTimeFactory* factory);
    
    /// Set the current time factory for the messenger.
    void SetTimeFactory(const G4String& factory);
    
    /// Get the list of time factories available to the messenger.
    G4String GetTimeFactories();
    
    /// Create a new generator using the current generator factories.
    EDepSim::PrimaryGenerator* CreateGenerator();

private:
    EDepSim::UserPrimaryGeneratorAction*  fAction; 
    
    /// The list of available kinematics factories;
    std::map<G4String,EDepSim::VKinematicsFactory*> fKinematicsFactories;

    /// The current kinematics factory
    EDepSim::VKinematicsFactory* fKinematics;

    /// The list of available count factories;
    std::map<G4String,EDepSim::VCountFactory*> fCountFactories;

    /// The current count factory
    EDepSim::VCountFactory* fCount;

    /// The list of available position factories;
    std::map<G4String,EDepSim::VPositionFactory*> fPositionFactories;

    /// The current position factory
    EDepSim::VPositionFactory* fPosition;

    /// The list of available time factories;
    std::map<G4String,EDepSim::VTimeFactory*> fTimeFactories;

    /// The current time factory
    EDepSim::VTimeFactory* fTime;

    G4UIdirectory* fDir;
    G4UIdirectory* fKinematicsDir;
    G4UIdirectory* fCountDir;
    G4UIdirectory* fPositionDir;
    G4UIdirectory* fTimeDir;

    G4UIcmdWithoutParameter* fClearCMD;
    G4UIcmdWithoutParameter* fAddCMD;
    G4UIcommand* fCombineCMD;
    G4UIcmdWithAString* fSetKinematicsCMD;
    G4UIcmdWithAString* fSetCountCMD;
    G4UIcmdWithAString* fSetPositionCMD;
    G4UIcmdWithAString* fSetTimeCMD;
    G4UIcmdWithABool* fAllowEmptyEventsCMD;
    G4UIcmdWithABool* fAddFakeGeantinoCMD;

};
#endif
