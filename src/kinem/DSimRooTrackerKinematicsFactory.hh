#ifndef DSimRooTrackerKinematicsFactory_hh_seen
#define DSimRooTrackerKinematicsFactory_hh_seen

#include "kinem/DSimVKinematicsFactory.hh"

class G4VPrimaryGenerator;
class DSimRooTrackerKinematicsGenerator;

class DSimRooTrackerKinematicsFactory : public DSimVKinematicsFactory {
public:
    DSimRooTrackerKinematicsFactory(DSimUserPrimaryGeneratorMessenger* fParent);
    virtual ~DSimRooTrackerKinematicsFactory();

    /// Return a new generator enclosing the current factory state.  The new
    /// generator method is pure virtual so it must be implemented by derived
    /// classes.
    virtual DSimVKinematicsGenerator* GetGenerator();

    /// Set the input file to read.
    virtual void SetInputFile(const G4String& name) {fInputFile=name;}

    /// Get the input file to read.
    virtual const G4String& GetInputFile() const {return fInputFile;}
    
    /// Set the generator name.  This is the name of the program that
    /// generated the rooTracker tree, and will be saved as documentation in
    /// the output file.
    virtual void SetGeneratorName(const G4String& name) {fGeneratorName = name;}

    /// Get the generator name.
    virtual const G4String& GetGeneratorName() const {return fGeneratorName;}

    /// Get the tree name.
    virtual const G4String& GetTreeName() const {return fTreeName;}

    /// Set the tree name.  This is the path of the rooTracker tree in the
    /// input root file.
    virtual void SetTreeName(const G4String& name) {fTreeName = name;}

    /// Set the order that events in the input file will be used.
    virtual void SetOrder(const G4String& order) {fOrder = order;}

    /// Get the order that events in the input file will be used.
    virtual G4String GetOrder() {return fOrder;}

    /// Set the first event to read.
    virtual void SetFirstEvent(int f) {fFirstEvent=f;}

    /// Get the first event to read.
    virtual int GetFirstEvent() const {return fFirstEvent;}

    virtual void SetNewValue(G4UIcommand* command,G4String newValue);

private:
    /// The root file with the RooTracker tree.
    G4String fInputFile;

    /// The path to the tree in the root file.
    G4String fTreeName;

    /// The name of the generater that created the rooTracker tree (eg GENIE,
    /// NEUT, &c).
    G4String fGeneratorName;

    /// The order that events in the input file will be used.
    G4String fOrder;

    /// The first event to read in the file.
    int fFirstEvent;

    G4UIcmdWithAString* fInputFileCMD;
    G4UIcmdWithAString* fTreeNameCMD;
    G4UIcmdWithAString* fGeneratorNameCMD;
    G4UIcmdWithAString* fOrderCMD;
    G4UIcmdWithAnInteger* fFirstEventCMD;
};
#endif
