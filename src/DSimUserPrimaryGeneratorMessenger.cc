#include "DSimUserPrimaryGeneratorAction.hh"
#include "DSimUserPrimaryGeneratorMessenger.hh"
#include "DSimException.hh"

#include "kinem/DSimPrimaryGenerator.hh"
#include "kinem/DSimCombinationGenerator.hh"
#include "kinem/DSimVKinematicsFactory.hh"
#include "kinem/DSimGPSKinematicsFactory.hh"
#include "kinem/DSimRooTrackerKinematicsFactory.hh"

#include "kinem/DSimVCountFactory.hh"
#include "kinem/DSimFixedCountFactory.hh"
#include "kinem/DSimMeanCountFactory.hh"

#include "kinem/DSimVPositionFactory.hh"
#include "kinem/DSimFreePositionFactory.hh"
#include "kinem/DSimFixedPositionFactory.hh"
#include "kinem/DSimUniformPositionFactory.hh"
#include "kinem/DSimDensityPositionFactory.hh"

#include "kinem/DSimVTimeFactory.hh"
#include "kinem/DSimFixedTimeFactory.hh"
#include "kinem/DSimSpillTimeFactory.hh"


#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>

#include <TCaptLog.hxx>

DSimUserPrimaryGeneratorMessenger::DSimUserPrimaryGeneratorMessenger(
    DSimUserPrimaryGeneratorAction* gen)
    : fAction(gen) { 

    /////////////////////////////////////////
    // The directories must be created first.
    /////////////////////////////////////////

    fDir = new G4UIdirectory("/generator/");
    fDir->SetGuidance("Control of the particle generator.");

    fCountDir = new G4UIdirectory("/generator/count/");
    fCountDir->SetGuidance("Control number of primary vertices per event.");

    fKinematicsDir = new G4UIdirectory("/generator/kinematics/");
    fKinematicsDir->SetGuidance("Control kinematics generators.");

    fPositionDir = new G4UIdirectory("/generator/position/");
    fPositionDir->SetGuidance("Control vertex position the generators.");

    fTimeDir = new G4UIdirectory("/generator/time/");
    fTimeDir->SetGuidance("Control vertex time the generators.");

    /////////////////////////////////////////
    // The factories must be created after the directories.
    /////////////////////////////////////////

    AddKinematicsFactory(new DSimGPSKinematicsFactory(this));
    AddKinematicsFactory(new DSimRooTrackerKinematicsFactory(this));

    AddCountFactory(new DSimFixedCountFactory(this));
    AddCountFactory(new DSimMeanCountFactory(this));

    AddPositionFactory(new DSimFreePositionFactory(this));
    AddPositionFactory(new DSimFixedPositionFactory(this));
    AddPositionFactory(new DSimUniformPositionFactory(this));
    AddPositionFactory(new DSimDensityPositionFactory(this));
    
    AddTimeFactory(new DSimFixedTimeFactory(this));
    AddTimeFactory(new DSimSpillTimeFactory(this));

    /////////////////////////////////////////
    // The UI commands must be created after the factories.
    /////////////////////////////////////////

    fClearCMD = new G4UIcmdWithoutParameter("/generator/clear",this);
    fClearCMD->SetGuidance("Clear the current list of vertex generators.");

    fAddCMD = new G4UIcmdWithoutParameter("/generator/add",this);
    fAddCMD->SetGuidance("Add a generator using the current factories.");

    fCombineCMD = new G4UIcmdWithoutParameter("/generator/combine",this);
    fCombineCMD->SetGuidance("Combine the previous two vertices into one.");

    fSetKinematicsCMD 
        = new G4UIcmdWithAString("/generator/kinematics/set",this);
    fSetKinematicsCMD->SetGuidance("Set the current kinematics factory.");
    fSetKinematicsCMD->SetParameterName("Factory",false);
    fSetKinematicsCMD->SetCandidates(GetKinematicsFactories());

    fSetCountCMD 
        = new G4UIcmdWithAString("/generator/count/set",this);
    fSetCountCMD->SetGuidance("Set the current count factory.");
    fSetCountCMD->SetParameterName("Factory",false);
    fSetCountCMD->SetCandidates(GetCountFactories());

    fSetPositionCMD 
        = new G4UIcmdWithAString("/generator/position/set",this);
    fSetPositionCMD->SetGuidance("Set the current position factory.");
    fSetPositionCMD->SetParameterName("Factory",false);
    fSetPositionCMD->SetCandidates(GetPositionFactories());

    fSetTimeCMD 
        = new G4UIcmdWithAString("/generator/time/set",this);
    fSetTimeCMD->SetGuidance("Set the current time factory.");
    fSetTimeCMD->SetParameterName("Factory",false);
    fSetTimeCMD->SetCandidates(GetTimeFactories());

    fAllowEmptyEventsCMD
        = new G4UIcmdWithABool("/generator/allowEmptyEvents",this);
    fAllowEmptyEventsCMD->SetGuidance("If true, then generate events even"
                                      " if they don't contain a vertex.");
    
    fAddFakeGeantinoCMD
        = new G4UIcmdWithABool("/generator/addFakeGeantino",this);
    fAddFakeGeantinoCMD->SetGuidance("If true, then guarantee a primary vertex"
                                     " by adding a geantino.");
    
    //////////////////////////////////
    // Set default values for the factories.
    SetKinematicsFactory("gps");
    SetCountFactory("fixed");
    SetPositionFactory("free");
    SetTimeFactory("spill");

}

DSimUserPrimaryGeneratorMessenger::~DSimUserPrimaryGeneratorMessenger() {
    delete fDir;
    delete fCountDir;
    delete fKinematicsDir;
    delete fPositionDir;
    delete fTimeDir;
    delete fClearCMD;
    delete fAddCMD;
    delete fCombineCMD;
    delete fSetKinematicsCMD;
    delete fSetCountCMD;
    delete fSetPositionCMD;
    delete fSetTimeCMD;
    delete fAllowEmptyEventsCMD;
    delete fAddFakeGeantinoCMD;
}

void DSimUserPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, 
                                                     G4String newValue) { 
    if (command == fClearCMD) {
        fAction->ClearGenerators();
    }
    else if (command == fAddCMD) {
        DSimPrimaryGenerator* generator = CreateGenerator();
        fAction->AddGenerator(generator);
    }
    else if (command == fCombineCMD) {
        DSimCombinationGenerator* generator = new DSimCombinationGenerator();
        fAction->AddGenerator(generator);
    }
    else if (command == fSetKinematicsCMD) {
        SetKinematicsFactory(newValue);
    }
    else if (command == fSetCountCMD) {
        SetCountFactory(newValue);
    }
    else if (command == fSetPositionCMD) {
        SetPositionFactory(newValue);
    }
    else if (command == fSetTimeCMD) {
        SetTimeFactory(newValue);
    }
    else if (command == fAllowEmptyEventsCMD) {
        fAction->SetAllowEmptyEvents(
            fAllowEmptyEventsCMD->GetNewBoolValue(newValue));
    }
    else if (command == fAddFakeGeantinoCMD) {
        fAction->SetAddFakeGeantino(
            fAddFakeGeantinoCMD->GetNewBoolValue(newValue));
    }
    else {
        DSimError("DSimUserPrimaryGeneratorMessenger:: "
                    "Unimplemented command");
    }
}

DSimPrimaryGenerator* DSimUserPrimaryGeneratorMessenger::CreateGenerator() {
    DSimVKinematicsGenerator* kine = fKinematics->GetGenerator();
    DSimVCountGenerator* count = fCount->GetGenerator();
    DSimVPositionGenerator* position = fPosition->GetGenerator();
    DSimVTimeGenerator* time = fTime->GetGenerator();
    DSimPrimaryGenerator* gen
        = new DSimPrimaryGenerator(kine,count,position,time);
    CaptLog("#############################################");
    CaptLog("# Create a new DSimPrimaryGenerator: " 
             << gen->GetName());
    CaptLog("#############################################");
    return gen;
}

G4String DSimUserPrimaryGeneratorMessenger::GetPath() {
    return fDir->GetCommandPath();
}

void DSimUserPrimaryGeneratorMessenger::AddKinematicsFactory(
    DSimVKinematicsFactory* factory) {
    fKinematicsFactories[factory->GetName()] = factory;
}

void DSimUserPrimaryGeneratorMessenger::SetKinematicsFactory(
    const G4String& name) {
    std::map<G4String,DSimVKinematicsFactory*>::const_iterator p
        = fKinematicsFactories.find(name);
    if (p != fKinematicsFactories.end()) {
        fKinematics = p->second;
        CaptLog("  Kinematics factory set to: " << fKinematics->GetName());
    }
    else {
        DSimError("  Illegal kinematics factory name.");
    }
}

G4String DSimUserPrimaryGeneratorMessenger::GetKinematicsFactories() {
    G4String list = "";
    for (std::map<G4String,DSimVKinematicsFactory*>::const_iterator p 
             = fKinematicsFactories.begin();
         p != fKinematicsFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}

void DSimUserPrimaryGeneratorMessenger::AddCountFactory(
    DSimVCountFactory* factory) {
    fCountFactories[factory->GetName()] = factory;
}

void DSimUserPrimaryGeneratorMessenger::SetCountFactory(
    const G4String& name) {
    std::map<G4String,DSimVCountFactory*>::const_iterator p
        = fCountFactories.find(name);
    if (p != fCountFactories.end()) {
        fCount = p->second;
        CaptLog("  Count factory set to: " << fCount->GetName());
    }
    else {
        DSimError("  Illegal count factory name.");
    }
}

G4String DSimUserPrimaryGeneratorMessenger::GetCountFactories() {
    G4String list = "";
    for (std::map<G4String,DSimVCountFactory*>::const_iterator p 
             = fCountFactories.begin();
         p != fCountFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}

void DSimUserPrimaryGeneratorMessenger::AddPositionFactory(
    DSimVPositionFactory* factory) {
    fPositionFactories[factory->GetName()] = factory;
}

void DSimUserPrimaryGeneratorMessenger::SetPositionFactory(
    const G4String& name) {
    std::map<G4String,DSimVPositionFactory*>::const_iterator p
        = fPositionFactories.find(name);
    if (p != fPositionFactories.end()) {
        fPosition = p->second;
        CaptLog("  Position factory set to: " << fPosition->GetName());
    }
    else {
        DSimError("  Illegal position factory name.");
    }
}

G4String DSimUserPrimaryGeneratorMessenger::GetPositionFactories() {
    G4String list = "";
    for (std::map<G4String,DSimVPositionFactory*>::const_iterator p 
             = fPositionFactories.begin();
         p != fPositionFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}

void DSimUserPrimaryGeneratorMessenger::AddTimeFactory(
    DSimVTimeFactory* factory) {
    fTimeFactories[factory->GetName()] = factory;
}

void DSimUserPrimaryGeneratorMessenger::SetTimeFactory(
    const G4String& name) {
    std::map<G4String,DSimVTimeFactory*>::const_iterator p
        = fTimeFactories.find(name);
    if (p != fTimeFactories.end()) {
        fTime = p->second;
        CaptLog("  Time factory set to: " << fTime->GetName());
    }
    else {
        DSimError("  Illegal time factory name.");
    }
}

G4String DSimUserPrimaryGeneratorMessenger::GetTimeFactories() {
    G4String list = "";
    for (std::map<G4String,DSimVTimeFactory*>::const_iterator p 
             = fTimeFactories.begin();
         p != fTimeFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}
