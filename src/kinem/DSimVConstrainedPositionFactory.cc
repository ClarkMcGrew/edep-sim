#include "kinem/DSimVConstrainedPositionFactory.hh"
#include "kinem/DSimVConstrainedPositionGenerator.hh"

DSimVConstrainedPositionFactory::DSimVConstrainedPositionFactory(
    G4String name,
    DSimUserPrimaryGeneratorMessenger* parent,
    bool makeDirectory)
    : DSimVPositionFactory(name,parent,makeDirectory),
      fCurrent(NULL) {

    fPositionSampleVolumeCMD 
        = new G4UIcmdWithAString(CommandName("sample"),this);
    fPositionSampleVolumeCMD->SetGuidance("[REQUIRED] "
                                          " Set volume to sample for a "
                                          "primary vertex.");
    fPositionSampleVolumeCMD->SetParameterName("SampleVolume",false);

    fPositionClearCMD = new G4UIcmdWithoutParameter(CommandName("clear"),
                                                   this);
    fPositionClearCMD->SetGuidance("Start a new vertex generator.");

    fPositionVolumeCMD = new G4UIcmdWithAString(CommandName("volume"),this);
    fPositionVolumeCMD->SetGuidance("Set the name of the vertex volume.");
    fPositionVolumeCMD->SetParameterName("PositionVolume",false);

    fPositionNotVolumeCMD = new G4UIcmdWithAString(CommandName("notVolume"),
                                                 this);
    fPositionNotVolumeCMD->SetGuidance("Set the name of the vertex"
                                     " volume to avoid.");
    fPositionNotVolumeCMD->SetParameterName("NotPositionVolume",false);

    fPositionMaterialCMD = new G4UIcmdWithAString(CommandName("material"),
                                                this);
    fPositionMaterialCMD->SetGuidance("Set the name of the vertex material.");
    fPositionMaterialCMD->SetParameterName("PositionMaterial",false);

    fPositionNotMaterialCMD 
        = new G4UIcmdWithAString(CommandName("notMaterial"),this);
    fPositionNotMaterialCMD->SetGuidance("Set the name of the"
                                       " material to avoid.");
    fPositionNotMaterialCMD->SetParameterName("PositionNotMaterial",false);

    fPositionMinXCMD = new G4UIcmdWithADoubleAndUnit(CommandName("minX"),
                                                this);
    fPositionMinXCMD->SetGuidance("Set the minimum position of the vertex.");
    fPositionMinXCMD->SetParameterName("X",false);
    fPositionMinXCMD->SetUnitCategory("Length");

    fPositionMinYCMD = new G4UIcmdWithADoubleAndUnit(CommandName("minY"),
                                                this);
    fPositionMinYCMD->SetGuidance("Set the minimum position of the vertex.");
    fPositionMinYCMD->SetParameterName("Y",false);
    fPositionMinYCMD->SetUnitCategory("Length");

    fPositionMinZCMD = new G4UIcmdWithADoubleAndUnit(CommandName("minZ"),
                                                this);
    fPositionMinZCMD->SetGuidance("Set the minimum position of the vertex.");
    fPositionMinZCMD->SetParameterName("Z",false);
    fPositionMinZCMD->SetUnitCategory("Length");

    fPositionMinTCMD = new G4UIcmdWithADoubleAndUnit(CommandName("minT"),
                                                this);
    fPositionMinTCMD->SetGuidance("Set the minimum time of the vertex.");
    fPositionMinTCMD->SetParameterName("T",false);
    fPositionMinTCMD->SetUnitCategory("Time");


    fPositionMaxXCMD = new G4UIcmdWithADoubleAndUnit(CommandName("maxX"),
                                                this);
    fPositionMaxXCMD->SetGuidance("Set the maximum position of the vertex.");
    fPositionMaxXCMD->SetParameterName("X",false);
    fPositionMaxXCMD->SetUnitCategory("Length");

    fPositionMaxYCMD = new G4UIcmdWithADoubleAndUnit(CommandName("maxY"),
                                                this);
    fPositionMaxYCMD->SetGuidance("Set the maximum position of the vertex.");
    fPositionMaxYCMD->SetParameterName("Y",false);
    fPositionMaxYCMD->SetUnitCategory("Length");

    fPositionMaxZCMD = new G4UIcmdWithADoubleAndUnit(CommandName("maxZ"),
                                                this);
    fPositionMaxZCMD->SetGuidance("Set the maximum position of the vertex.");
    fPositionMaxZCMD->SetParameterName("Z",false);
    fPositionMaxZCMD->SetUnitCategory("Length");

    fPositionMaxTCMD = new G4UIcmdWithADoubleAndUnit(CommandName("maxT"),
                                                this);
    fPositionMaxTCMD->SetGuidance("Set the maximum position of the vertex.");
    fPositionMaxTCMD->SetParameterName("T",false);
    fPositionMaxTCMD->SetUnitCategory("Time");
}

DSimVConstrainedPositionFactory::~DSimVConstrainedPositionFactory() {
    delete fPositionSampleVolumeCMD;
    delete fPositionClearCMD;
    delete fPositionVolumeCMD;
    delete fPositionNotVolumeCMD;
    delete fPositionMaterialCMD;
    delete fPositionNotMaterialCMD;
    delete fPositionMinXCMD;
    delete fPositionMinYCMD;
    delete fPositionMinZCMD;
    delete fPositionMinTCMD;
    delete fPositionMaxXCMD;
    delete fPositionMaxYCMD;
    delete fPositionMaxZCMD;
    delete fPositionMaxTCMD;
}

void DSimVConstrainedPositionFactory::SetNewValue(G4UIcommand* command, 
                                                 G4String newValue) { 

    if (!fCurrent) {
        fCurrent = dynamic_cast<DSimVConstrainedPositionGenerator*>(
            CreateGenerator());
    }

    if (command == fPositionClearCMD) {
        if (fCurrent) {
            fCurrent->ClearPositionTests();
        }
    }
    else if (command == fPositionSampleVolumeCMD) {
        if (fCurrent) {
            fCurrent->SetVolumeName(newValue);
        }
    }
    else if (command == fPositionVolumeCMD) {
        if (fCurrent) {
            fCurrent->CheckVolumeName(newValue);
        }
    }
    else if (command == fPositionNotVolumeCMD) {
        if (fCurrent) {
            fCurrent->CheckNotVolumeName(newValue);
        }
    }
    else if (command == fPositionMaterialCMD) {
        if (fCurrent) {
            fCurrent->CheckVolumeMaterial(newValue);
        }
    }
    else if (command == fPositionNotMaterialCMD) {
        if (fCurrent) {
            fCurrent->CheckNotVolumeMaterial(newValue);
        }
    }
    else if (command == fPositionMinXCMD) {
        if (fCurrent) {
            fCurrent->CheckMinX(fPositionMinXCMD->GetNewDoubleValue(newValue));
        }
    }
    else if (command == fPositionMinYCMD) {
        if (fCurrent) {
            fCurrent->CheckMinY(fPositionMinYCMD->GetNewDoubleValue(newValue));
        }
    }
    else if (command == fPositionMinZCMD) {
        if (fCurrent) {
            fCurrent->CheckMinZ(fPositionMinZCMD->GetNewDoubleValue(newValue));
        }
    }
    else if (command == fPositionMaxXCMD) {
        if (fCurrent) {
            fCurrent->CheckMaxX(fPositionMaxXCMD->GetNewDoubleValue(newValue));
        }
    }
    else if (command == fPositionMaxYCMD) {
        if (fCurrent) {
            fCurrent->CheckMaxY(fPositionMaxYCMD->GetNewDoubleValue(newValue));
        }
    }
    else if (command == fPositionMaxZCMD) {
        if (fCurrent) {
            fCurrent->CheckMaxZ(fPositionMaxZCMD->GetNewDoubleValue(newValue));
        }
    }
}

DSimVPositionGenerator* DSimVConstrainedPositionFactory::GetGenerator() {
    DSimVPositionGenerator* stash = fCurrent;
    if (!stash) stash = CreateGenerator();
    fCurrent = NULL;
    return stash;
}
