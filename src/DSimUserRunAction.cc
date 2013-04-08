////////////////////////////////////////////////////////////
// $Id: DSimUserRunAction.cc,v 1.12 2011/07/19 20:55:13 mcgrew Exp $
//

#include <ctime>
#include <sys/time.h>

#include <Randomize.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4VVisManager.hh>
#include <G4ios.hh>
#include <G4Timer.hh>

#include <TCaptLog.hxx>

#include "DSimUserRunAction.hh"
#include "DSimUserRunActionMessenger.hh"

DSimUserRunAction::DSimUserRunAction() 
    : fStartTime("invalid"), fStopTime("invalid"), fSubrunId(-1) {
    fTimer = new G4Timer;
    fMessenger= new DSimUserRunActionMessenger(this);
}

DSimUserRunAction::~DSimUserRunAction() {
    delete fTimer;
    delete fMessenger;
}

void DSimUserRunAction::BeginOfRunAction(const G4Run* aRun) {
    // Initialize the run header
    time_t ltime = time(NULL);
    fStartTime = ctime(&ltime);
    fTimer->Start();

#ifdef UPDATE_VISUALIZATION
    if (G4VVisManager::GetConcreteInstance()) {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        UI->ApplyCommand("/vis/scene/notifyHandlers");
    } 
#endif

    CaptLog("### Run " << aRun->GetRunID() << " starting.");

}

void DSimUserRunAction::EndOfRunAction(const G4Run* aRun) {
    fTimer->Stop();
    time_t ltime = time(NULL);
    fStopTime = ctime(&ltime);

    CaptLog("### Run " << aRun->GetRunID() << " ending.");
    CaptLog("Number of events = " << aRun->GetNumberOfEvent());
    CaptLog(*fTimer);


#ifdef UPDATE_VISUALIZATION
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  
    if( pVVisManager ) {
        G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update"); 
    }
#endif
}

void DSimUserRunAction::SetSeed(long seed) {
    if (seed<0) seed = -seed;
    CaptLog("### Random seed number set to: " << seed);
    G4Random::setTheSeed(seed);
}


long DSimUserRunAction::GetSeed(void) const {
    return G4Random::getTheSeed();
}

void DSimUserRunAction::SetTimeSeed() {
    long seed = 0;
#ifdef __USE_POSIX
    struct timeval buffer;
    gettimeofday(&buffer,NULL);
    CaptLog("### Set seed from local time: " << ctime(&buffer.tv_sec));
    CaptLog("###      Micro-second offset: " << buffer.tv_usec);
    seed = long(buffer.tv_sec + buffer.tv_usec);
#else
#warning Using seconds since epoch to seed the random generator.
    seed = time(NULL);
#endif
    // Make sure the seed is positive;
    if (seed<0) seed = -seed;
    // Make sure the seed isn't too large.
    seed = seed % (2l<<(8*sizeof(long)-4));
    // Make sure the seed is odd and not zero.
    seed += (seed % 2) + 1;
    SetSeed(long(seed));
}

void DSimUserRunAction::SetDetSimRunId(int v) {
    G4RunManager* manager = G4RunManager::GetRunManager();
    manager->SetRunIDCounter(v);
}

void DSimUserRunAction::SetDetSimSubrunId(int v) {
    fSubrunId = v;
}

int DSimUserRunAction::GetDetSimSubrunId() const {
    return fSubrunId;
}
