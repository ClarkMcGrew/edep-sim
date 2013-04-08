#include <map>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include <TROOT.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLorentzVector.h>
#include <TParticlePDG.h>

#include <THandle.hxx>
#include <THit.hxx>
#include <TG4Trajectory.hxx>
#include <TG4VHit.hxx>
#include <TG4HitSegment.hxx>

#include <HEPUnits.hxx>

#include <nd280EventLoop.hxx>

class TParticleRange: public CP::TEventLoopFunction {
public:
    TParticleRange() {
        fHistogramRange = 2000*unit::mm;
        fEnergyRange = 0;
        fParticleName = "";
        fParticleRange = NULL;
        fIntimeEnergy = NULL;
        fLateEnergy = NULL;
        fInitialEnergy = NULL;
        fMomentumRange = NULL;
    }

    virtual ~TParticleRange() {};

    void Usage(void) {
        std::cout << "    -O range=<val> The histogram range in mm."
                  << std::endl;
    }

    virtual bool SetOption(std::string option,std::string value="") {
        if (option == "range") {
            std::istringstream input(value);
            input >> fHistogramRange;
        }
        else if (option == "energy") {
            std::istringstream input(value);
            input >> fEnergyRange;
        }
        else return false;
        return true;
    }

    // Histogram the distance between the first and last trajectory point for
    // the primary particle.
    bool operator () (CP::TEvent& event) {
        CP::THandle<CP::TG4TrajectoryContainer> trajs 
            = event.Get<CP::TG4TrajectoryContainer>("truth/G4Trajectories");
        CP::THandle<CP::TG4Trajectory> primary = trajs->GetTrajectory(1);
        // The head of the track.
        TLorentzVector head = primary->GetInitialPosition();
        TLorentzVector tail = primary->GetFinalPosition();
        int pdgCode = primary->GetPDGEncoding();
        int trajId = 1;
        TLorentzVector initialMomentum = primary->GetInitialMomentum();
        std::string particleName = primary->GetParticleName();

        if (fEnergyRange < 1) fEnergyRange = 1.1*initialMomentum.E();

        // Find the particle range.
        for (CP::TG4TrajectoryContainer::iterator i = trajs->begin();
             i != trajs->end();
             ++i) {
            if (i->first < trajId) continue;
            CP::TG4Trajectory& current = i->second;
            if (trajId != current.GetParentId()) continue;
            if (pdgCode != current.GetPDGEncoding()) continue;
            TLorentzVector currHead = current.GetInitialPosition();
            if ((currHead-tail).Vect().Mag()>1*unit::mm) continue;
            if (std::abs(currHead.T()-tail.T())>1*unit::ns) continue;
            // This is a continuation of the primary, so update.
            trajId = i->first;
            tail = current.GetFinalPosition();
        }
        double range = (tail-head).Vect().Mag();
        if (!fParticleRange) {
            std::string name = "particleRange";
            std::ostringstream title;
            title << "Range for " << initialMomentum.E()/unit::MeV
                  << " MeV " << particleName;
            fParticleRange = new TH1F(name.c_str(), title.str().c_str(),
                                      100, 0.0, fHistogramRange);
            fParticleRange->SetXTitle("Range (mm)");
        }
        fParticleRange->Fill(range/unit::mm);

        if (!fMomentumRange) {
            std::string name = "momentumRange";
            std::ostringstream title;
            title << "Ranges for " << particleName << " vs Momentum ";
            double maxMom = 500*unit::MeV;
            const TParticlePDG* part = primary->GetParticle();
            if (part) {
                maxMom = part->Mass()*unit::GeV;
                double ene = part->Mass()*unit::GeV + 500*unit::MeV;
                maxMom = std::sqrt(ene*ene - maxMom*maxMom);
            }
               
            fMomentumRange = new TH2F(name.c_str(), title.str().c_str(),
                                      100, 0.0, maxMom, 
                                      1000, 0.0, fHistogramRange);
            fMomentumRange->SetXTitle("Momentum (MeV/c)");
            fMomentumRange->SetYTitle("Range (mm)");
        }
        fMomentumRange->Fill(initialMomentum.P()/unit::MeV, range);

        // Find the collected energy.
        CP::THandle<CP::TG4HitContainer> g4Hits = 
            event.Get<CP::TG4HitContainer>("truth/g4Hits/p0d");
        if (!g4Hits) return false;

        double collectedEnergy = 0.0;
        double lateEnergy = 0.0;
        for (CP::TG4HitContainer::iterator g4Hit = g4Hits->begin();
             g4Hit != g4Hits->end();
             ++g4Hit) {
            CP::TG4HitSegment* hitSeg 
                = dynamic_cast<CP::TG4HitSegment*>(*g4Hit);
            if (std::abs(head.T()-hitSeg->GetStopT()) > 50*unit::ns) {
                lateEnergy += hitSeg->GetEnergyDeposit();
                continue;
            }
            collectedEnergy += hitSeg->GetEnergyDeposit();
        }
        if (!fIntimeEnergy) {
            std::string name = "intimeEnergy";
            std::ostringstream title;
            title << "Deposited Energy for " << initialMomentum.E()/unit::MeV 
                  << " MeV " << particleName;
            fIntimeEnergy = new TH1F(name.c_str(), title.str().c_str(),
                                     100, 0.0, fEnergyRange);
            fIntimeEnergy->SetXTitle("Collected Energy (MeV)");

            name = "lateEnergy";
            fLateEnergy = new TH1F(name.c_str(), 
                                     (title.str() + " (late)").c_str(),
                                     100, 0.0, fEnergyRange);
            fLateEnergy->SetXTitle("Collected Energy (MeV)");
        }

        if (!fInitialEnergy) {
            double eneMax = std::max(fEnergyRange, 500*unit::MeV);
            fInitialEnergy = new TH1F("initialEnerrgy", "Initial Energy",
                                      100, 0.0, eneMax);
            fInitialEnergy->SetXTitle("Energy (MeV)");
        }

        fIntimeEnergy->Fill(collectedEnergy);
        fLateEnergy->Fill(lateEnergy);
        const TParticlePDG* part = primary->GetParticle();
        if (part) fInitialEnergy->Fill(initialMomentum.E()
                                       - part->Mass()*unit::GeV);

        return false;
    }

private:
    /// The histogram range
    double fHistogramRange;
    
    // the range of the energy histogram.
    double fEnergyRange;

    // The range of the particle.
    TH1F* fParticleRange;

    // The initial energy distribution of the particles.
    TH1F* fInitialEnergy;

    // The intime deposited energy.
    TH1F* fIntimeEnergy;

    // The late deposited energy.
    TH1F* fLateEnergy;

    // A profile of the  range vs the momentum
    TH2F* fMomentumRange;

    // The particle being processed
    std::string fParticleName;

};

int main(int argc, char **argv) {
    TParticleRange userCode;
    nd280EventLoop(argc,argv,userCode);
}
