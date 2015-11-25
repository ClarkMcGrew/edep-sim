#ifndef MiniCaptExposedBuilder_hh_Seen
#define MiniCaptExposedBuilder_hh_Seen

#include "DSimBuilder.hh"
class G4LogicalVolume;

#include <string>

/// Construct the exposed part of the miniCAPTAIN detector.  This builds
/// exposed parts of the TPC that are in the argon gas.  The exposed volume is
/// returned as a cylinder filled with liquid argon.
class MiniCaptExposedBuilder : public DSimBuilder {
public:
    MiniCaptExposedBuilder(G4String name, DSimBuilder* parent)
        : DSimBuilder(name,parent) {Init();};
    virtual ~MiniCaptExposedBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Return the offset of the intended origin of the volume relative to the
    /// center of the logical volume.  To get the origin at the right location
    /// (say originPosition), the logical volume should be positioned at
    /// originPosition-GetOffset().  The offset is defined by the bottom of
    /// the wire plane assembly (a decision will be made in the future as to
    /// whether this is the bottom of the grid, or the bottom of the V plane.
    /// This means that the wires for the V plane are at a (very) small
    /// positive z coordinate.
    G4ThreeVector GetOffset() {return fOffset;}

    /// Get the radius of the exposed region.  This is calculated.
    double GetRadius();

    /// Get the height of the exposed region.  This is calculated.
    double GetHeight();

private:
    void Init(void);

    G4ThreeVector fOffset;
};
#endif

