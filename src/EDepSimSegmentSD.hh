////////////////////////////////////////////////////////////
// $Id: EDepSim::SegmentSD.hh,v 1.3 2007/01/01 05:36:12 mcgrew Exp $
//

#ifndef EDepSim_SegmentSD_h
#define EDepSim_SegmentSD_h 1

#include "G4VSensitiveDetector.hh"
#include "EDepSimLog.hh"
#include "EDepSimHitSegment.hh"

class G4HCofThisEvent;
class G4Step;

/// A sensitive detector to create EDepSim::HitSegment based hits.
namespace EDepSim {class SegmentSD;}
class EDepSim::SegmentSD : public G4VSensitiveDetector {
    
public:
    SegmentSD(G4String name);
    virtual ~SegmentSD();
    
    void Initialize(G4HCofThisEvent*);
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    void EndOfEvent(G4HCofThisEvent*);

    /// Set the maximum sagitta for the EDepSim::HitSegment objects created by
    /// this sensitive detector.
    void SetMaximumHitSagitta(double sagitta) {
        EDepSimLog("Set max segment sagitta to " << sagitta
                << " for " << GetName());
        fMaximumHitSagitta = sagitta;
    }
    double GetMaximumHitSagitta(void) {return fMaximumHitSagitta;}

    /// Set the maximum separation between deposits for the
    /// EDepSim::HitSegment objects created by this sensitive
    /// detector.
    void SetMaximumHitSeparation(double separation) {
        EDepSimLog("Set max segment separation to " << separation
                << " for " << GetName());
        fMaximumHitSeparation = separation;
    }
    double GetMaximumHitSeparation(void) {return fMaximumHitSeparation;}
    
    /// Set the maximum length for the EDepSim::HitSegment objects
    /// created by this sensitive detector.
    void SetMaximumHitLength(double length) {
        EDepSimLog("Set max segment length to " << length
            << " for " << GetName());
        fMaximumHitLength = length;
    }
    double GetMaximumHitLength(void) {return fMaximumHitLength;}
    
private:
    /// The collection of hits that is being filled in the current event.  It
    /// is constructed in Initialize, filled in ProcessHits, and added the the
    /// event in EndOfEvent.
    EDepSim::HitSegment::HitSegmentCollection* fHits;

    /// The hit collection id of fHits
    int fHCID;

    /// The maximum allowed sagitta;
    double fMaximumHitSagitta;

    /// The maximum distance between deposits that can be combined.
    double fMaximumHitSeparation;
    
    /// The maximum allowed length;
    double fMaximumHitLength;

    /// The last hit that was found.
    int fLastHit;
};

#endif

