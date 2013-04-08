#include "kinem/DSimVPrimaryFactory.hh"

DSimVPrimaryFactory::DSimVPrimaryFactory(
    G4String subdir, G4String name,
    DSimUserPrimaryGeneratorMessenger* parent,
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

DSimVPrimaryFactory::~DSimVPrimaryFactory() {
    if (fDir) delete fDir;
}

G4String DSimVPrimaryFactory::GetPath() const {
    G4String dirName = fParent->GetPath() 
        + GetSubDir() + "/" 
        + GetName() + "/";
    return dirName;
}

G4String DSimVPrimaryFactory::GetName() const {
    return fName;
}

G4String DSimVPrimaryFactory::GetSubDir() const {
    return fSubDirName;
}

