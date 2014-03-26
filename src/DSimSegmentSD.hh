////////////////////////////////////////////////////////////
// $Id: DSimSegmentSD.hh,v 1.3 2007/01/01 05:36:12 mcgrew Exp $
//

#ifndef DSimSegmentSD_h
#define DSimSegmentSD_h 1

#include "G4VSensitiveDetector.hh"
#include "DSimLog.hh"
#include "DSimHitSegment.hh"

class G4HCofThisEvent;
class G4Step;

/// A sensitive detector to create DSimHitSegment based hits.
class DSimSegmentSD : public G4VSensitiveDetector
{
    
public:
    DSimSegmentSD(G4String name);
    virtual ~DSimSegmentSD();
    
    void Initialize(G4HCofThisEvent*);
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    void EndOfEvent(G4HCofThisEvent*);

    /// Set the maximum sagitta for the DSimHitSegment objects created by
    /// this sensitive detector.
    void SetMaximumHitSagitta(double sagitta) {
        DSimLog("Set max segment sagitta to " << sagitta
                << " for " << GetName());
        fMaximumHitSagitta = sagitta;
    }
    double GetMaximumHitSagitta(void) {return fMaximumHitSagitta;}

    /// Set the maximum length for the DSimHitSegment objects created by this
    /// sensitive detector.
    void SetMaximumHitLength(double length) {
        DSimLog("Set max segment length to " << length
                << " for " << GetName());
        fMaximumHitLength = length;
    }
    double GetMaximumHitLength(void) {return fMaximumHitLength;}

private:
    /// The collection of hits that is being filled in the current event.  It
    /// is constructed in Initialize, filled in ProcessHits, and added the the
    /// event in EndOfEvent.
    DSimHitSegmentCollection* fHits;

    /// The hit collection id of fHits
    int fHCID;

    /// The maximum allowed sagitta;
    double fMaximumHitSagitta;

    /// The maximum allowed length;
    double fMaximumHitLength;

    /// The last hit that was found.
    int fLastHit;
};

#endif

