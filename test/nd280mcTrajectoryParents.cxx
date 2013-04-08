#include <algorithm>
#include <memory>
#include <iostream>
#include <vector>

#include <TDataVector.hxx>
#include <TG4HitSegment.hxx>
#include <TG4Trajectory.hxx>
#include <TG4VHit.hxx>
#include <THandle.hxx>
#include <TEvent.hxx>
#include <TND280Log.hxx>
#include <TND280Output.hxx>

#include <nd280EventLoop.hxx>

/// User code for the event loop.  This can be copied and used as a skeleton
/// for generic user loop programs.
class TUserLoop: public CP::TEventLoopFunction {
public:
    /// Initialize any class specific variables, but most of the work can be
    /// done in Initialize.  Don't create histograms here!
    TUserLoop();
    virtual ~TUserLoop();
  
    /// Print a usage message.  This is generally called when there is a
    /// command line input error.
    void Usage(void) {}
  
    /// Set an option and return true if it is valid.  This is called by the
    /// event loop command line argument processing code for each "-O
    /// [name]=[value]" option found on the command line.  If the command line
    /// has "-O [name]" without a value, then the value string will be equal
    /// to "".  This must return false if the option was not correctly
    /// processed.
    virtual bool SetOption(std::string option,std::string value="") { 
        return false; 
    }

    /// Called for each event inside the event loop, and returns true if the
    /// event should be saved to the output file.  If the remainder of the
    /// current file should be skipped, this should through the
    /// ENextEventLoopFile exception.
    bool operator () (CP::TEvent&);

    /// Called after the arguments are processes by before reading the first
    /// event.  The output file is open so any histograms will be added to the
    /// output file.
    void Initialize(void);
  
private:
  
};

TUserLoop::TUserLoop() {}

TUserLoop::~TUserLoop() {}

void TUserLoop::Initialize(void) {

}

bool TUserLoop::operator() (CP::TEvent& event) {
    
    // Get the trajectory container
    CP::THandle<CP::TG4TrajectoryContainer> trajectories
        = event.Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");
    
    // Get the unique set of Primaries from the G4Hits
    CP::THandle<CP::TDataVector> detectorHits
        = event.Get<CP::TDataVector>("truth/g4Hits");
    
    std::vector<int> primaries;
    std::vector<int> contributors;
    
    // Get all the primaries
    for (CP::TDataVector::iterator d = detectorHits->begin();
         d != detectorHits->end(); ++d){
        
        CP::THandle<CP::TG4HitContainer> hits = 
            (*d)->Get<CP::TG4HitContainer>(".");
        
        for (CP::TG4HitContainer::iterator h = hits->begin();
             h != hits->end(); ++h){
            CP::TG4HitSegment* hit = dynamic_cast<CP::TG4HitSegment*>(*h);
            primaries.push_back(hit->GetPrimaryId());
            std::copy(hit->GetContributors().begin(), 
                      hit->GetContributors().end(),
                      std::back_inserter(contributors));
        }
    }
        
    std::sort(primaries.begin(),primaries.end());
    primaries.erase(std::unique(primaries.begin(), 
                                primaries.end()),
                    primaries.end());
        
    std::sort(contributors.begin(),contributors.end());
    contributors.erase(std::unique(contributors.begin(), 
                                   contributors.end()),
                       contributors.end());
    
    if (!primaries.empty()) {
        // Check that all primaries were saved
        ND280Log("Found " << primaries.size() << " primaries ");
        for (std::vector<int>::iterator p = primaries.begin();
             p != primaries.end(); ++p) {
            if (trajectories->GetTrajectory(*p)) continue;
            ND280Log("Event: " << event.GetContext()
                     << " Missing TG4VHit primary:" << *p);
        }
    }

    if (!contributors.empty()) {
        // Check that all contributors were saved    
        ND280Log("Found " << contributors.size() << " contributors ");
        for (std::vector<int>::iterator p = contributors.begin();
             p != contributors.end(); ++p) {
            if (trajectories->GetTrajectory(*p)) continue;
            ND280Log("Event: " << event.GetContext()
                     << " missing TG4VHit contributor:" << *p);
        }
    }

    return false;

}

int main(int argc, char **argv) {
    TUserLoop userLoop;
    nd280EventLoop(argc,argv,userLoop);
}
