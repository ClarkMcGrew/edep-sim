////////////////////////////////////////////////////////////
// $Id: DSimHitSegment.hh,v 1.5 2011/06/29 04:35:53 mcgrew Exp $

#ifndef DSimHitSegment_h
#define DSimHitSegment_h 

#include <vector>

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>

class G4Step;
class G4Track;

#include "DSimVolumeId.hh"
#include "TG4HitSegment.hxx"

/// This is a base class used to save G4Hit objects into a root output file.
/// It contains the global position of the starting point and stopping point
/// of the track segment that created the hit, plus the any fields provided by
/// CP::TG4VHit.  The electronics response is simulated in a separate program
/// (library) which will construct the digitized hits.  This class is used
/// with the DSimSegmentSD sensitive detector class, and can be requested
/// using the "segment" name from DSimSensitiveDetectorFactor::MakeSD().
class DSimHitSegment : public CP::TG4HitSegment, public G4VHit {
public:
    /// Create a new hit segment with a maximum allowed sagitta and length.
    /// The default values are set so that normally, a scintillator element
    /// will only have a single hit for a through going track (& delta-rays).
    DSimHitSegment(double maxSagitta = 1*mm, double maxLength = 5*mm);
    virtual ~DSimHitSegment();
    
    inline void* operator new(size_t);
    inline void  operator delete(void*);
    
    /// Add the effects of a part of a step to this hit.
    virtual void AddStep(G4Step* theStep, double start=0.0, double end=1.0);

    /// Hits for the same primary particle, in the same physical volume belong
    /// in the same hit.
    virtual bool SameHit(G4Step* theStep);

    /// Disambiguate  the draw and  print methods  to distinguish  between the
    /// G4VHit and  TObject definitions.
    virtual void Draw();
    virtual void Print();
    virtual void Draw(const char*) {}
    virtual void Print(const char*) const {}

    /// Provide public access to the contributors for internal G4 classes.
    std::vector<int>& GetContributors() {return fContributors;}

    /// Find the distance from the starting point to stoping point of the
    /// track.
    virtual double GetLength() const;

protected:
    /// Find the primary track ID for the current track.  This is the primary
    /// that is the ultimate parent of the current track.
    int FindPrimaryId(G4Track* theTrack);

    /// Find the maximum separation (the sagitta) between the current hit
    /// segment path points, and the straight line connecting the start and
    /// proposed new stop point.
    double FindSagitta(G4Step* theStep);

    /// Find the maximum distance from the hit segment to the new step that is
    /// proposed to be added to the hit segment. This is used to
    /// combine secondaries with a parent track.
    double FindSeparation(G4Step* theStep);

    /// The sagitta tolerance for the segment.
    double fMaxSagitta;

    /// The maximum length between the start and stop points of the segment.
    double fMaxLength;

private:
    /// The G4 physical volume that contains the hit.
    DSimVolumeId fHitVolume;

    /// The end points of the steps that make up this hit.  This is used to
    /// make sure that the current hit stays inside of it's allowed
    /// tolerances.
    std::vector<G4ThreeVector> fPath;
    
};

typedef
G4THitsCollection<DSimHitSegment> DSimHitSegmentCollection;

extern G4Allocator<DSimHitSegment> DSimHitSegmentAllocator;

inline void* DSimHitSegment::operator new(size_t) {
    void *aHit;
    aHit = (void *) DSimHitSegmentAllocator.MallocSingle();
    return aHit;
}


inline void DSimHitSegment::operator delete(void *aHit) {
    DSimHitSegmentAllocator.FreeSingle((DSimHitSegment*) aHit);
}

#endif
