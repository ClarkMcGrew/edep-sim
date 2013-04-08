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
        SElec::TSimplisticElec tpc("tpc","tpc", 0.5*unit::mm);
        tpc.AddVolumeName("TPC");
        tpc.AddVolumeName("Pad_");
        tpc.GenerateHits(event);

        SElec::TSimplisticElec p0d("p0d","p0d", 
                                   1*unit::cm);
        p0d.AddVolumeName("P0D_");
        p0d.AddVolumeName("Bar_");
        p0d.GenerateHits(event);

        SElec::TSimplisticElec fgd("fgd","fgd");
        fgd.AddVolumeName("FGD");
        fgd.AddVolumeName("Bar_");
        fgd.GenerateHits(event);

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

