#include "kinem/EDepSimVPrimaryFactory.hh"

EDepSim::VPrimaryFactory::VPrimaryFactory(
    G4String subdir, G4String name,
    EDepSim::UserPrimaryGeneratorMessenger* parent,
    bool makeDirectory) {
    fSubDirName = subdir;
    fName = name;
    fParent = parent;
    if (makeDirectory) {
        fDir = new G4UIdirectory(GetPath());
        std::string guidance = "Control of the " + fName 
            + " " + fSubDirName + " factory.";
        fDir->SetGuidance(guidance.c_str());
    }
    else fDir = NULL;
}

EDepSim::VPrimaryFactory::~VPrimaryFactory() {
    if (fDir) delete fDir;
}

G4String EDepSim::VPrimaryFactory::GetPath() const {
    G4String dirName = fParent->GetPath() 
        + GetSubDir() + "/" 
        + GetName() + "/";
    return dirName;
}

G4String EDepSim::VPrimaryFactory::GetName() const {
    return fName;
}

G4String EDepSim::VPrimaryFactory::GetSubDir() const {
    return fSubDirName;
}

