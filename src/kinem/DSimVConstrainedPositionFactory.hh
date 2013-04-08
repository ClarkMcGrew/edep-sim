#ifndef DSimVConstrainedPositionFactory_hh_seen
#define DSimVConstrainedPositionFactory_hh_seen

#include "kinem/DSimVPositionFactory.hh"

class DSimVConstrainedPositionGenerator;
class DSimVPositionGenerator;

class DSimVConstrainedPositionFactory : public DSimVPositionFactory {
public:
    DSimVConstrainedPositionFactory(G4String name, 
                        DSimUserPrimaryGeneratorMessenger* fParent,
                        bool makeDirectory = true);
    virtual ~DSimVConstrainedPositionFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes, but the derived generator must return the value from
    /// DSimVConstrainedPositionFactory::GetGenerator().
    virtual DSimVPositionGenerator* GetGenerator();

    /// Create a new generator that can be initialized.  This is a pure
    /// virtual function so it must be implemented in the derived class.
    virtual DSimVPositionGenerator* CreateGenerator() = 0;

    void SetNewValue(G4UIcommand*, G4String);

private:

    DSimVConstrainedPositionGenerator* fCurrent;

    G4UIcmdWithAString* fPositionSampleVolumeCMD;
    G4UIcmdWithoutParameter*   fPositionClearCMD;
    G4UIcmdWithAString* fPositionVolumeCMD;
    G4UIcmdWithAString* fPositionNotVolumeCMD;
    G4UIcmdWithAString* fPositionMaterialCMD;
    G4UIcmdWithAString* fPositionNotMaterialCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinXCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinYCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinZCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMinTCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxXCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxYCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxZCMD;
    G4UIcmdWithADoubleAndUnit* fPositionMaxTCMD;

};
#endif
