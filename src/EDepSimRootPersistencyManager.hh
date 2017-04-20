////////////////////////////////////////////////////////////
// $Id: EDepSim::RootPersistencyManager.hh,v 1.31 2011/09/06 18:58:35 mcgrew Exp $
// 
#ifndef EDepSim_RootPersistencyManager_hh_seen
#define EDepSim_RootPersistencyManager_hh_seen

#include <string>
#include <vector>
#include <map>

class TFile;
class TTree;
class TGeoManager;

#include "EDepSimPersistencyManager.hh"

/// Provide a root output for the geant 4 events.  This just takes the summary
/// from EDepSim::PersistencyManager and dumps it as a tree.
namespace EDepSim {class RootPersistencyManager;}
class EDepSim::RootPersistencyManager : public EDepSim::PersistencyManager {
public:
    /// Creates a root persistency manager.  Through the "magic" of
    /// G4VPersistencyManager the ultimate base class, this declared to the G4
    /// persistency management system.  You can only have one active
    /// persistency class at any give moment.
    RootPersistencyManager();
    virtual ~RootPersistencyManager();

    /// Return true if the ROOT output file is active.  This means that the
    /// output file is open and ready to accept data.
    bool IsOpen();

    /// Return a pointer to the current TFile.
    TFile* GetTFile() const {return fOutput;}

    /// Stores an event to the output file.
    virtual G4bool Store(const G4Event* anEvent);
    virtual G4bool Store(const G4Run* aRun);
    virtual G4bool Store(const G4VPhysicalVolume* aWorld);

    /// Retrieve information from a file.  These are not implemented.
    virtual G4bool Retrieve(G4Event *&e) {e=NULL; return false;}
    virtual G4bool Retrieve(G4Run* &r) {r=NULL; return false;}
    virtual G4bool Retrieve(G4VPhysicalVolume* &w) {w=NULL; return false;}

    /// Interface with PersistencyMessenger (open and close the
    /// database).
    virtual G4bool Open(G4String dbname);
    virtual G4bool Close(void);

private:
    /// Make the MC Header and add it to truth.
    void MakeMCHeader(const G4Event* src);

private:
    /// The ROOT output file that events are saved into.
    TFile *fOutput;

    /// The event tree that contains the output events.
    TTree *fEventTree;

    /// The number of events saved to the output file since the last write.
    int fEventsNotSaved;

};
#endif
