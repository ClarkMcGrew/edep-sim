/// This is not an electronics simulation.
///
/// Provide a fake electronics simulation to help test detSim.  This produces
/// all of the required hits and digits, but makes no attempt to match real
/// electronics behaviour (beyond stuff required for testing).
///
/// DO NOT USE THIS TO GENERATE PHYSICS RESULTS.
///
#include <TROOT.h>
#include <TH1F.h>
#include <TH2F.h>

#include <eventLoop.hxx>
#include <HEPUnits.hxx>

#include "TSimplisticElec.hxx"

class TDetSimElec: public CP::TEventLoopFunction {
public:
    TDetSimElec() {
        fQuiet = false;
    }

    virtual ~TDetSimElec() {};

    void Usage(void) {
        std::cout << "    -O quiet    Very little output"
                  << std::endl;
    }

    virtual bool SetOption(std::string option,std::string value="") {
        if (option == "quiet") {
            fQuiet = true;
            return true;
        }
        return false;
    }

    bool operator () (CP::TEvent& event) {
        SElec::TSimplisticElec tpc("drift","drift", 1.0*unit::mm, fDriftHits);
        tpc.AddVolumeName("Drift");
        tpc.AddSkipName("Drift_0/");
        tpc.GenerateHits(event);

        return true;
    }

    void Initialize() {
        fDriftHits = new TH1F("driftHits", "Energy deposited in TPC",
                              100, 0.15, 0.25);
    }

private:
    bool fQuiet;
    TH1* fDriftHits;
};

int main(int argc, char **argv) {
    TDetSimElec userCode;
    eventLoop(argc,argv,userCode);
}

