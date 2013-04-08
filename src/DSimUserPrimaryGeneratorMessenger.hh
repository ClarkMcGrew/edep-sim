////////////////////////////////////////////////////////////
// $Id: DSimUserPrimaryGeneratorMessenger.hh,v 1.9 2012/05/10 16:27:26 mcgrew Exp $
//
#ifndef DSimUserPrimaryGeneratorMessenger_h
#define DSimUserPrimaryGeneratorMessenger_h 1

#include <map>

#include "G4UImessenger.hh"
#include "globals.hh"

class DSimUserPrimaryGeneratorAction;
class DSimVKinematicsFactory;
class DSimVCountFactory;
class DSimVPositionFactory;
class DSimVTimeFactory;
class DSimPrimaryGenerator;

class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class DSimUserPrimaryGeneratorMessenger: public G4UImessenger {
public:
    DSimUserPrimaryGeneratorMessenger(DSimUserPrimaryGeneratorAction*);
    virtual ~DSimUserPrimaryGeneratorMessenger();

    /// Handle messages from the UI processor.
    void SetNewValue(G4UIcommand*, G4String);

    /// Get the base directory for the messenger commands.
    G4String GetPath();

    /// Add a new kinematics factory to the messenger.
    void AddKinematicsFactory(DSimVKinematicsFactory* factory);
    
    /// Set the current kinematics factory for the messenger.
    void SetKinematicsFactory(const G4String& factory);
    
    /// Get the list of kinematics factories available to the messenger.
    G4String GetKinematicsFactories();
    
    /// Add a new count factory to the messenger.
    void AddCountFactory(DSimVCountFactory* factory);
    
    /// Set the current count factory for the messenger.
    void SetCountFactory(const G4String& factory);
    
    /// Get the list of count factories available to the messenger.
    G4String GetCountFactories();
    
    /// Add a new position factory to the messenger.
    void AddPositionFactory(DSimVPositionFactory* factory);
    
    /// Set the current position factory for the messenger.
    void SetPositionFactory(const G4String& factory);
    
    /// Get the list of position factories available to the messenger.
    G4String GetPositionFactories();

    /// Add a new time factory to the messenger.
    void AddTimeFactory(DSimVTimeFactory* factory);
    
    /// Set the current time factory for the messenger.
    void SetTimeFactory(const G4String& factory);
    
    /// Get the list of time factories available to the messenger.
    G4String GetTimeFactories();
    
    /// Create a new generator using the current generator factories.
    DSimPrimaryGenerator* CreateGenerator();

private:
    DSimUserPrimaryGeneratorAction*  fAction; 
    
    /// The list of available kinematics factories;
    std::map<G4String,DSimVKinematicsFactory*> fKinematicsFactories;

    /// The current kinematics factory
    DSimVKinematicsFactory* fKinematics;

    /// The list of available count factories;
    std::map<G4String,DSimVCountFactory*> fCountFactories;

    /// The current count factory
    DSimVCountFactory* fCount;

    /// The list of available position factories;
    std::map<G4String,DSimVPositionFactory*> fPositionFactories;

    /// The current position factory
    DSimVPositionFactory* fPosition;

    /// The list of available time factories;
    std::map<G4String,DSimVTimeFactory*> fTimeFactories;

    /// The current time factory
    DSimVTimeFactory* fTime;

    G4UIdirectory* fDir;
    G4UIdirectory* fKinematicsDir;
    G4UIdirectory* fCountDir;
    G4UIdirectory* fPositionDir;
    G4UIdirectory* fTimeDir;

    G4UIcmdWithoutParameter* fClearCMD;
    G4UIcmdWithoutParameter* fAddCMD;
    G4UIcmdWithoutParameter* fCombineCMD;
    G4UIcmdWithAString* fSetKinematicsCMD;
    G4UIcmdWithAString* fSetCountCMD;
    G4UIcmdWithAString* fSetPositionCMD;
    G4UIcmdWithAString* fSetTimeCMD;
    G4UIcmdWithABool* fAllowEmptyEventsCMD;
    G4UIcmdWithABool* fAddFakeGeantinoCMD;

};
#endif
