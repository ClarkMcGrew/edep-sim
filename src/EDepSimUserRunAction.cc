////////////////////////////////////////////////////////////
// $Id: EDepSim::UserRunAction.cc,v 1.12 2011/07/19 20:55:13 mcgrew Exp $
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

#include <EDepSimLog.hh>

#include "EDepSimUserRunAction.hh"
#include "EDepSimUserRunActionMessenger.hh"

EDepSim::UserRunAction::UserRunAction() 
    : fStartTime("invalid"), fStopTime("invalid"), fSubrunId(-1) {
    fTimer = new G4Timer;
    fMessenger= new EDepSim::UserRunActionMessenger(this);
}

EDepSim::UserRunAction::~UserRunAction() {
    delete fTimer;
    delete fMessenger;
}

void EDepSim::UserRunAction::BeginOfRunAction(const G4Run* aRun) {
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

    EDepSimLog("### Run " << aRun->GetRunID() << " starting.");

}

void EDepSim::UserRunAction::EndOfRunAction(const G4Run* aRun) {
    fTimer->Stop();
    time_t ltime = time(NULL);
    fStopTime = ctime(&ltime);

    EDepSimLog("### Run " << aRun->GetRunID() << " ending.");
    EDepSimLog("Number of events = " << aRun->GetNumberOfEvent());
    EDepSimLog(*fTimer);


#ifdef UPDATE_VISUALIZATION
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  
    if( pVVisManager ) {
        G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update"); 
    }
#endif
}

void EDepSim::UserRunAction::SetSeed(long seed) {
    if (seed<0) seed = -seed;
    EDepSimLog("### Random seed number set to: " << seed);
    G4Random::setTheSeed(seed);
}


long EDepSim::UserRunAction::GetSeed(void) const {
    return G4Random::getTheSeed();
}

void EDepSim::UserRunAction::SetTimeSeed() {
    long seed = 0;
#ifdef __USE_POSIX
    struct timeval buffer;
    gettimeofday(&buffer,NULL);
    EDepSimLog("### Set seed from local time: " << ctime(&buffer.tv_sec));
    EDepSimLog("###      Micro-second offset: " << buffer.tv_usec);
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
    // Condition the seed.
    for (int i=0; i<10000000; ++i) G4UniformRand();
}

void EDepSim::UserRunAction::SetDetSimRunId(int v) {
    G4RunManager* manager = G4RunManager::GetRunManager();
    manager->SetRunIDCounter(v);
}

void EDepSim::UserRunAction::SetDetSimSubrunId(int v) {
    fSubrunId = v;
}

int EDepSim::UserRunAction::GetDetSimSubrunId() const {
    return fSubrunId;
}
