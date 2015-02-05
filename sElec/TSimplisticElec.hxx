#ifndef TSimplisticElec_hxx_seen
#define TSimplisticElec_hxx_seen

#include <string>
#include <vector>

#include <TEvent.hxx>
#include <HEPUnits.hxx>

class TH2;

namespace SElec {
    class TSimplisticElec;
}

/// A base class for implementing the simplistic electronics simulation.  This
/// converts the deposited energy saved in TG4HitSegment objects into TMCHit
/// objects.  The time of the TMCHit is the earliest time saved in the hit.
/// The charge of the TMCHit is the deposited energy multiplied by a constant
/// (Default: 1).
class SElec::TSimplisticElec {
public:
    TSimplisticElec(std::string g4Hits, std::string hits, 
                    double maxStep=1*unit::cm,
                    TH2* depositHist=NULL);
    virtual ~TSimplisticElec();

    void GenerateHits(CP::TEvent& event);

    /// Add a volume name to the list of required sub-strings found in the
    /// geometry id path.  If any one of these sub-strings is missing, the
    /// volume is skipped.
    void AddVolumeName(std::string name) {
        fVolumeNames.push_back(name);
    }

    void SetMultiplier(double m) {
        fEnergyMultiplier = m;
    }

private:
    /// The name of the G4Hits to get out of the event.
    std::string fG4Hits;

    /// The name of the THitSelection to save in the event.
    std::string fHits;
    
    /// The energy multiplier.
    double fEnergyMultiplier;

    /// The maximum step to take.
    double fMaxStep;

    /// A histogram of energy deposited in a hit versus length of track making
    /// the deposit.
    TH2* fDepositHist;

    /// Strings that must be part of the volume name for it to be considered
    /// valid.
    std::vector<std::string> fVolumeNames;
};
#endif
