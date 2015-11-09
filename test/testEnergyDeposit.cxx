
#include <TH1F.h>
#include <TH2F.h>
#include <TLorentzVector.h>
#include <TParticlePDG.h>

#include <THandle.hxx>
#include <TG4VHit.hxx>
#include <TG4HitSegment.hxx>

#include <HEPUnits.hxx>

#include <eventLoop.hxx>

class TEnergyDeposit: public CP::TEventLoopFunction {
public:
    TEnergyDeposit() {
        fEnergyDeposit = NULL;
        fSecondaryDeposit = NULL;
    }

    virtual ~TEnergyDeposit() {};

    void Usage(void) {
        std::cout << "    No options extra."
                  << std::endl;
    }

    virtual bool SetOption(std::string option,std::string value="") {
        return false;
    }

    // Histogram the distance between the first and last trajectory point for
    // the primary particle.
    bool operator () (CP::TEvent& event) {
        // Find the collected energy.
        CP::THandle<CP::TG4HitContainer> g4Hits = 
            event.Get<CP::TG4HitContainer>("truth/g4Hits/drift");
        if (!g4Hits) return false;

        if (!fEnergyDeposit) {
            fEnergyDeposit = new TH1F("energyDeposit",
                                      "Total Deposited Energy",
                                      100, 0.0, 100.0);
        }

        if (!fSecondaryDeposit) {
            fSecondaryDeposit
                = new TH1F("secondaryDeposit",
                           "Fraction of total energy that is non-ionizing",
                           100, 0.0, 1.0);
        }

        double energyDeposit = 0.0;
        double secondaryDeposit = 0.0;
        for (CP::TG4HitContainer::iterator g4Hit = g4Hits->begin();
             g4Hit != g4Hits->end();
             ++g4Hit) {
            CP::TG4HitSegment* hitSeg 
                = dynamic_cast<CP::TG4HitSegment*>(*g4Hit);
            if (!hitSeg) {
                std::cout << "Hit is not a segment" << std::endl;
                continue;
            }
            energyDeposit += hitSeg->GetEnergyDeposit();
            secondaryDeposit += hitSeg->GetSecondaryDeposit();
        }
        fEnergyDeposit->Fill(energyDeposit);
        fSecondaryDeposit->Fill(secondaryDeposit/energyDeposit);
        
        return false;
    }

private:

    TH1F* fEnergyDeposit;
    TH1F* fSecondaryDeposit;
};

int main(int argc, char **argv) {
    TEnergyDeposit userCode;
    eventLoop(argc,argv,userCode);
}
