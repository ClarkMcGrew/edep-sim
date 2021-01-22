#include "EDepSimUserPrimaryGeneratorAction.hh"
#include "EDepSimUserPrimaryGeneratorMessenger.hh"
#include "EDepSimException.hh"

#include "kinem/EDepSimPrimaryGenerator.hh"
#include "kinem/EDepSimCombinationGenerator.hh"
#include "kinem/EDepSimVKinematicsFactory.hh"
#include "kinem/EDepSimGPSKinematicsFactory.hh"
#include "kinem/EDepSimRooTrackerKinematicsFactory.hh"
#include "kinem/EDepSimNuMIRockKinematicsFactory.hh"

#include "kinem/EDepSimVCountFactory.hh"
#include "kinem/EDepSimFixedCountFactory.hh"
#include "kinem/EDepSimMeanCountFactory.hh"

#include "kinem/EDepSimVPositionFactory.hh"
#include "kinem/EDepSimFreePositionFactory.hh"
#include "kinem/EDepSimFixedPositionFactory.hh"
#include "kinem/EDepSimUniformPositionFactory.hh"
#include "kinem/EDepSimDensityPositionFactory.hh"

#include "kinem/EDepSimVTimeFactory.hh"
#include "kinem/EDepSimFixedTimeFactory.hh"
#include "kinem/EDepSimFreeTimeFactory.hh"
#include "kinem/EDepSimSpillTimeFactory.hh"


#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcommand.hh>

#include <EDepSimLog.hh>

EDepSim::UserPrimaryGeneratorMessenger::UserPrimaryGeneratorMessenger(
    EDepSim::UserPrimaryGeneratorAction* gen)
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

    AddKinematicsFactory(new EDepSim::GPSKinematicsFactory(this));
    AddKinematicsFactory(new EDepSim::NuMIRockKinematicsFactory(this));
    AddKinematicsFactory(new EDepSim::RooTrackerKinematicsFactory(this));

    AddCountFactory(new EDepSim::FixedCountFactory(this));
    AddCountFactory(new EDepSim::MeanCountFactory(this));

    AddPositionFactory(new EDepSim::FreePositionFactory(this));
    AddPositionFactory(new EDepSim::FixedPositionFactory(this));
    AddPositionFactory(new EDepSim::UniformPositionFactory(this));
    AddPositionFactory(new EDepSim::DensityPositionFactory(this));

    AddTimeFactory(new EDepSim::FixedTimeFactory(this));
    AddTimeFactory(new EDepSim::FreeTimeFactory(this));
    AddTimeFactory(new EDepSim::SpillTimeFactory(this));

    /////////////////////////////////////////
    // The UI commands must be created after the factories.
    /////////////////////////////////////////

    fClearCMD = new G4UIcmdWithoutParameter("/generator/clear",this);
    fClearCMD->SetGuidance("Clear the current list of vertex generators.");

    fAddCMD = new G4UIcmdWithoutParameter("/generator/add",this);
    fAddCMD->SetGuidance("Add a generator using the current factories.");

    fCombineCMD = new G4UIcommand("/generator/combine",this);
    fCombineCMD->SetGuidance("Combine the previous two vertices into one.");
    fCombineCMD->SetParameter(new G4UIparameter("src",'i',false));
    fCombineCMD->SetParameter(new G4UIparameter("dest",'i',false));
    fCombineCMD->SetParameter(new G4UIparameter("relative",'b',true));
    fCombineCMD->GetParameter(2)->SetDefaultValue("false");

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

    fAllowPartialEventsCMD
        = new G4UIcmdWithABool("/generator/allowPartialEvents",this);
    fAllowEmptyEventsCMD->SetGuidance("If true, then generate the final event"
                                      " even if it ran out of interactions"
                                      " in the input kinematics file.");

    //////////////////////////////////
    // Set default values for the factories.
    SetKinematicsFactory("gps");
    SetCountFactory("fixed");
    SetPositionFactory("free");
    SetTimeFactory("fixed");

}

EDepSim::UserPrimaryGeneratorMessenger::~UserPrimaryGeneratorMessenger() {
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
    delete fAllowPartialEventsCMD;
}

void EDepSim::UserPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,
                                                    G4String newValue) {
    if (command == fClearCMD) {
        fAction->ClearGenerators();
    }
    else if (command == fAddCMD) {
        EDepSim::PrimaryGenerator* generator = CreateGenerator();
        fAction->AddGenerator(generator);
    }
    else if (command == fCombineCMD) {
        int src, dest;
        std::string relative;
        std::istringstream is(newValue);
        is >> src >> dest >> relative;
        EDepSim::CombinationGenerator* generator
            = new EDepSim::CombinationGenerator(
                src,dest,G4UIcommand::ConvertToBool(relative.c_str()));
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
    else if (command == fAllowPartialEventsCMD) {
        fAction->SetAllowPartialEvents(
            fAllowPartialEventsCMD->GetNewBoolValue(newValue));
    }
    else {
        EDepSimThrow("EDepSim::UserPrimaryGeneratorMessenger:: "
                    "Unimplemented command");
    }
}

EDepSim::PrimaryGenerator*
EDepSim::UserPrimaryGeneratorMessenger::CreateGenerator() {
    EDepSim::VKinematicsGenerator* kine = fKinematics->GetGenerator();
    EDepSim::VCountGenerator* count = fCount->GetGenerator();
    EDepSim::VPositionGenerator* position = fPosition->GetGenerator();
    EDepSim::VTimeGenerator* time = fTime->GetGenerator();
    EDepSim::PrimaryGenerator* gen
        = new EDepSim::PrimaryGenerator(kine,count,position,time);
    EDepSimLog("#############################################");
    EDepSimLog("# Create a new EDepSim_PrimaryGenerator: "
             << gen->GetName());
    EDepSimLog("#############################################");
    return gen;
}

G4String EDepSim::UserPrimaryGeneratorMessenger::GetPath() {
    return fDir->GetCommandPath();
}

void EDepSim::UserPrimaryGeneratorMessenger::AddKinematicsFactory(
    EDepSim::VKinematicsFactory* factory) {
    fKinematicsFactories[factory->GetName()] = factory;
}

void EDepSim::UserPrimaryGeneratorMessenger::SetKinematicsFactory(
    const G4String& name) {
    std::map<G4String,EDepSim::VKinematicsFactory*>::const_iterator p
        = fKinematicsFactories.find(name);
    if (p != fKinematicsFactories.end()) {
        fKinematics = p->second;
        EDepSimLog("  Kinematics factory set to: " << fKinematics->GetName());
    }
    else {
        EDepSimThrow("  Illegal kinematics factory name.");
    }
}

G4String EDepSim::UserPrimaryGeneratorMessenger::GetKinematicsFactories() {
    G4String list = "";
    for (std::map<G4String,EDepSim::VKinematicsFactory*>::const_iterator p
             = fKinematicsFactories.begin();
         p != fKinematicsFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}

void EDepSim::UserPrimaryGeneratorMessenger::AddCountFactory(
    EDepSim::VCountFactory* factory) {
    fCountFactories[factory->GetName()] = factory;
}

void EDepSim::UserPrimaryGeneratorMessenger::SetCountFactory(
    const G4String& name) {
    std::map<G4String,EDepSim::VCountFactory*>::const_iterator p
        = fCountFactories.find(name);
    if (p != fCountFactories.end()) {
        fCount = p->second;
        EDepSimLog("  Count factory set to: " << fCount->GetName());
    }
    else {
        EDepSimThrow("  Illegal count factory name.");
    }
}

G4String EDepSim::UserPrimaryGeneratorMessenger::GetCountFactories() {
    G4String list = "";
    for (std::map<G4String,EDepSim::VCountFactory*>::const_iterator p
             = fCountFactories.begin();
         p != fCountFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}

void EDepSim::UserPrimaryGeneratorMessenger::AddPositionFactory(
    EDepSim::VPositionFactory* factory) {
    fPositionFactories[factory->GetName()] = factory;
}

void EDepSim::UserPrimaryGeneratorMessenger::SetPositionFactory(
    const G4String& name) {
    std::map<G4String,EDepSim::VPositionFactory*>::const_iterator p
        = fPositionFactories.find(name);
    if (p != fPositionFactories.end()) {
        fPosition = p->second;
        EDepSimLog("  Position factory set to: " << fPosition->GetName());
    }
    else {
        EDepSimThrow("  Illegal position factory name.");
    }
}

G4String EDepSim::UserPrimaryGeneratorMessenger::GetPositionFactories() {
    G4String list = "";
    for (std::map<G4String,EDepSim::VPositionFactory*>::const_iterator p
             = fPositionFactories.begin();
         p != fPositionFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}

void EDepSim::UserPrimaryGeneratorMessenger::AddTimeFactory(
    EDepSim::VTimeFactory* factory) {
    fTimeFactories[factory->GetName()] = factory;
}

void EDepSim::UserPrimaryGeneratorMessenger::SetTimeFactory(
    const G4String& name) {
    std::map<G4String,EDepSim::VTimeFactory*>::const_iterator p
        = fTimeFactories.find(name);
    if (p != fTimeFactories.end()) {
        fTime = p->second;
        EDepSimLog("  Time factory set to: " << fTime->GetName());
    }
    else {
        EDepSimThrow("  Illegal time factory name.");
    }
}

G4String EDepSim::UserPrimaryGeneratorMessenger::GetTimeFactories() {
    G4String list = "";
    for (std::map<G4String,EDepSim::VTimeFactory*>::const_iterator p
             = fTimeFactories.begin();
         p != fTimeFactories.end();
         ++p) {
        list = list + p->first + " ";
    }
    return list;
}
