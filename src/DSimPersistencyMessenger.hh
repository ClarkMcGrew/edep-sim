////////////////////////////////////////////////////////////
// $Id: DSimPersistencyMessenger.hh,v 1.12 2011/09/06 18:58:35 mcgrew Exp $
// 
// class description:
//
//      This is a messenger class for DSimPersistencyManager.
//      Implemented commands are following;
//
//  Commands : 
//    /db/open *            Open the output file
//    /db/close             Close the database
// 

#ifndef DSimPersistencyMessenger_h
#define DSimPersistencyMessenger_h 1

#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;

class DSimPersistencyManager;

class DSimPersistencyMessenger: public G4UImessenger {
public:
    DSimPersistencyMessenger(DSimPersistencyManager* persistencyMgr);
    virtual ~DSimPersistencyMessenger();
    
    void SetNewValue(G4UIcommand* command,G4String newValues);
    G4String GetCurrentValue(G4UIcommand* command);
    
private:
    DSimPersistencyManager* fPersistencyManager;
    
    G4UIdirectory*             fPersistencyDIR;
    G4UIdirectory*             fPersistencySetDIR;
    G4UIcmdWithAString*        fOpenCMD;   
    G4UIcmdWithoutParameter*   fCloseCMD;
    G4UIcmdWithADoubleAndUnit* fGammaThresholdCMD;
    G4UIcmdWithADoubleAndUnit* fNeutronThresholdCMD;
    G4UIcmdWithADoubleAndUnit* fLengthThresholdCMD;
    G4UIcmdWithABool*          fSaveAllPrimaryTrajectoriesCMD;
    G4UIcmdWithADoubleAndUnit* fTrajectoryPointAccuracyCMD;
    G4UIcmdWithAString*        fTrajectoryBoundaryCMD;
    G4UIcmdWithoutParameter*   fClearBoundariesCMD;

};
#endif

