#ifndef DSimSpillTimeGenerator_hh_seen
#define DSimSpillTimeGenerator_hh_seen

#include <vector>

#include "kinem/DSimVTimeGenerator.hh"

/// Generate an interaction time according to the spill parameters.  This
/// object is created using the DSimSpillTimeFactory where the spill
/// parameters can be set (there is more documentation there).  The time of
/// the interaction is adjusted for the neutrino travel time.
class DSimSpillTimeGenerator : public DSimVTimeGenerator {
public:
    /// Create the generatory.  The name is for documentation, the spillTime
    /// gives the start time of the first bunch at the center of the DSim
    /// hall, the bunchSeparation is the time between bunch starts, and the
    /// bunchLength is the time length of a bunch with respect to it's start
    /// time.
    DSimSpillTimeGenerator(G4String name, 
                            double spillTime,
                            double bunchSeparation,
                            double bunchLength,
                            const std::vector<double>& bunchPower);
    virtual ~DSimSpillTimeGenerator();

    /// Return the time of the event to be generated.
    double GetTime(const G4LorentzVector& vertex);

    /// Flag if the time should be forced.  This tells the calling routine
    /// that the time returned by this generator should override any times
    /// found in the original vertex.  This returns true, so that the
    /// generated spill time overrides the interaction time.
    virtual bool ForceTime();

private:
    /// The time that the neutrinos are crossing (0,0,0).
    double fSpillTime;
    
    /// The time between bunch starts.
    double fBunchSeparation;
    
    /// The length of a bunch.  This is the gaussian width of the bunch.
    double fBunchLength;

    /// The power in each bunch.
    std::vector<double> fBunchPower;

    /// The maximum power in a bunch.
    double fMaxPower;
};
#endif
