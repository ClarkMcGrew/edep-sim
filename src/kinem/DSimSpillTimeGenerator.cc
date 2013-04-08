#include "kinem/DSimVTimeGenerator.hh"
#include "kinem/DSimSpillTimeGenerator.hh"

#include <Randomize.hh>

#include <TCaptLog.hxx>

DSimSpillTimeGenerator::DSimSpillTimeGenerator(
    G4String name, double spillTime, double bunchSeparation,
    double bunchLength, const std::vector<double>& bunchPower)
    : DSimVTimeGenerator(name), fSpillTime(spillTime),
      fBunchSeparation(bunchSeparation), fBunchLength(bunchLength),
      fBunchPower(bunchPower) {
    if (fBunchPower.size()<1) fBunchPower.push_back(1.0);
    fMaxPower = 0.0;
    for (std::vector<double>::iterator b = fBunchPower.begin();
         b != fBunchPower.end();
         ++b) {
        if (fMaxPower<(*b)) fMaxPower = *b;
    }

    if (fMaxPower<0.001) {
        CaptError("Spill created without any power in bunchs");
    }
}

DSimSpillTimeGenerator::~DSimSpillTimeGenerator() {}

double DSimSpillTimeGenerator::GetTime(const G4LorentzVector& vtx) {
    // Choose a spill;
    int spills = fBunchPower.size();
    int spill = 0;
    do {
        spill = int(spills*G4UniformRand());
        if (spill>=spills) spill=spills-1;
        if (spill<0) spill = 0;
    } while (fBunchPower[spill]<fMaxPower*G4UniformRand());

    // Get the spill start time.  This assumes the neutrino wave is traveling
    // along the positive Z axis.
    double time = fSpillTime + vtx.z()/c_light + spill*fBunchSeparation;

    // Adjust the time in the bunch.
    double truncTime = G4RandGauss::shoot(0.0, 1.0);
    truncTime = std::min(std::max(-2.0,truncTime),2.0); 
    time += fBunchLength*truncTime;
    
    return time;
}

bool DSimSpillTimeGenerator::ForceTime() {
    return true;
}
