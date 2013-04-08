////////////////////////////////////////////////////////////
// $Id: DSimUserRunAction.hh,v 1.9 2011/07/19 20:55:13 mcgrew Exp $
//

#ifndef DSimUserRunAction_h
#define DSimUserRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Run;
class G4Timer;
class DSimUserRunActionMessenger;

class DSimUserRunAction : public G4UserRunAction {
public:
    DSimUserRunAction();
    virtual ~DSimUserRunAction();
    
public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    const G4Timer* GetRunTimer(void) const {return fTimer;};

    /// Set the seed to a new value.  This takes a long since the low-level
    /// random generate expects a long seed.
    void SetSeed(long);

    /// Get the seed that started the low level random generator.
    long GetSeed(void) const;

    /// Build a seed for the generator based on the system time.
    void SetTimeSeed();

    /// Set the DetSim Run Id to a specific value.  This is the first run id
    /// that will be used by GEANT.  GEANT will automatically increment the
    /// run id everytime it starts a new internal run.  The run id should be
    /// accessed through G4RunInfo (see G4RunManager).
    void SetDetSimRunId(int v);

    /// Set the DetSim Subrun Id to a specific value.  This has no internal
    /// meaning and is just copied to the output event.
    void SetDetSimSubrunId(int v);

    /// Get the subrun id value.
    int GetDetSimSubrunId() const;

private:
    /// The messenger for this action
    DSimUserRunActionMessenger* fMessenger;

    /// The time that the run was started.
    G4String fStartTime;
    
    /// The time that the run was stopped.
    G4String fStopTime;

    /// The running time for the run.
    G4Timer* fTimer;
    
    /// The cached value of the subrun id.
    int fSubrunId;
};
#endif
