////////////////////////////////////////////////////////////
//

#ifndef EDepSim_SurfaceSD_h
#define EDepSim_SurfaceSD_h 1

#include "G4VSensitiveDetector.hh"
#include "EDepSimLog.hh"
#include "EDepSimHitSurface.hh"

class G4HCofThisEvent;
class G4Step;

/// A sensitive detector to create hits at a surface (mostly optical surfaces).
namespace EDepSim {class SurfaceSD;}
class EDepSim::SurfaceSD : public G4VSensitiveDetector {

public:
    SurfaceSD(G4String name);
    virtual ~SurfaceSD();

    void Initialize(G4HCofThisEvent*);
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    void EndOfEvent(G4HCofThisEvent*);

private:
    /// The collection of hits that is being filled in the current event.  It
    /// is constructed in Initialize, filled in ProcessHits, and added the the
    /// event in EndOfEvent.
    EDepSim::HitSurface::HitSurfaceCollection* fHits;

    /// The hit collection id of fHits
    int fHCID;
};

#endif
