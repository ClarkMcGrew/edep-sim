/// This is not an electronics simulation.
///
/// Provide a fake electronics simulation to help test detSim.  This produces
/// all of the required hits and digits, but makes no attempt to match real
/// electronics behaviour (beyond stuff required for testing).
///
/// DO NOT USE THIS TO GENERATE PHYSICS RESULTS.
///
#include <TROOT.h>
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
        TH2* tpcHits = new TH2F("driftHits", "Energy deposited in TPC",
                                100, 0.0, 1000.0,
                                100, 0.0, 100.0);
        SElec::TSimplisticElec tpc("drift","drift", 1.0*unit::mm, tpcHits);
        tpc.AddVolumeName("Drift");
        tpc.GenerateHits(event);

        return true;
    }

    void Initialize() {}

private:
    bool fQuiet;

};

int main(int argc, char **argv) {
    TDetSimElec userCode;
    eventLoop(argc,argv,userCode);
}

