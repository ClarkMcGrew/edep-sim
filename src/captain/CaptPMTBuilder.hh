#ifndef CaptPMTBuilder_hh_Seen
#define CaptPMTBuilder_hh_Seen

#include "EDepSimBuilder.hh"

class G4LogicalVolume;

/// Construct an unrotated PMT.  In the local coordinate system, the
/// PMT points along the positive Z direction.
class CaptPMTBuilder : public EDepSim::Builder {
public:
    CaptPMTBuilder(G4String name, EDepSim::Builder* parent)
        : EDepSim::Builder(name,parent) {Init();};
    virtual ~CaptPMTBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Get or set the length of the base.  The base length measures from the
    /// face of the photocathode to the back of the PMT. 
    /// @{
    void SetBaseLength(double v) {fBaseLength = v;}
    double GetBaseLength() const {return fBaseLength;}
    /// @}

    /// Set the size of the PMT.
    /// @{
    void SetSize(double v) {fSize = v;}
    double GetSize() const {return fSize;}
    /// @}

    /// Set that the PMT is round
    void SetRound(bool v) {fRoundPMT = v;}
    bool IsRound() const {return fRoundPMT;}

private:
    void Init(void);

    /// The size of the PMT.
    double fSize;

    /// The length of the PMT base.
    double fBaseLength;

    /// Flag that the PMT is round (not square).
    bool fRoundPMT;
    
};
#endif
