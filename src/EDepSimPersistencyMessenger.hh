////////////////////////////////////////////////////////////
//

#ifndef EDepSim_PersistencyMessenger_h
#define EDepSim_PersistencyMessenger_h 1

#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;

namespace EDepSim {class PersistencyManager;}

namespace EDepSim {class PersistencyMessenger;}
class EDepSim::PersistencyMessenger: public G4UImessenger {
public:
    PersistencyMessenger(EDepSim::PersistencyManager* persistencyMgr);
    virtual ~PersistencyMessenger();

    void SetNewValue(G4UIcommand* command,G4String newValues);
    G4String GetCurrentValue(G4UIcommand* command);

private:
    EDepSim::PersistencyManager* fPersistencyManager;

    G4UIdirectory*             fPersistencyDIR;
    G4UIdirectory*             fPersistencySetDIR;
    G4UIcmdWithAString*        fOpenCMD;
    G4UIcmdWithoutParameter*   fCloseCMD;
    G4UIcmdWithADoubleAndUnit* fGammaThresholdCMD;
    G4UIcmdWithADoubleAndUnit* fNeutronThresholdCMD;
    G4UIcmdWithADoubleAndUnit* fLengthThresholdCMD;
    G4UIcmdWithABool*          fSaveAllPrimaryTrajectoriesCMD;
    G4UIcmdWithADoubleAndUnit* fSaveAllTrajectoriesCMD;
    G4UIcmdWithADoubleAndUnit* fTrajectoryPointAccuracyCMD;
    G4UIcmdWithADoubleAndUnit* fTrajectoryPointDepositCMD;
    G4UIcmdWithAString*        fTrajectoryBoundaryCMD;
    G4UIcmdWithoutParameter*   fClearBoundariesCMD;

};
#endif
