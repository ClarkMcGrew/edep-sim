#ifndef CaptCryostatBuilder_hh_Seen
#define CaptCryostatBuilder_hh_Seen

#include "DSimBuilder.hh"

class G4LogicalVolume;

/// Construct the cryostat.  This builds the vessel, as well as the argon gas
/// and liquid inside.  This builds the vessel directly and fills it with
/// argon gas.  The liquid argon is build by a separate constructor which will
/// contain the TPC.
/// 
/// \bug The current class is an extremely simplistic model.  I think the
/// cryostat is multi-walled, but I don't have the design documents right now.
/// The invariants for this class are the inner diameter and inner heights
/// which define the cold volume.  The outer diameter and height should be
/// calculated.
class CaptCryostatBuilder : public DSimBuilder {
public:
    CaptCryostatBuilder(G4String name, DSimBuilder* parent)
        : DSimBuilder(name,parent) {Init();};
    virtual ~CaptCryostatBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Get the total height of the cryostat.  This is used to position the
    /// cryostat in the world volume and is calculated from the cryostat
    /// dimensions.
    double GetHeight();

    /// Get the total diameter of the cryostat.  This is used to position the
    /// cryostat in the world volume and is calculated from the cryostat
    /// dimensions.
    double GetDiameter();

    /// Get (or set) the inner diameter of the cryostat.  This is the diameter
    /// of the cold volume.
    /// @{
    double GetInnerDiameter() const {return fInnerDiameter;}
    void SetInnerDiameter(double v) {fInnerDiameter = v;}
    /// @}

    /// Get (or set) the inner height of the cryostat.  This is the diameter
    /// of the cold volume.
    /// @{
    double GetInnerHeight() const {return fInnerHeight;}
    void SetInnerHeight(double v) {fInnerHeight = v;}
    /// @}

    /// Get (or set) the steel thickness for the cryostat.  
    /// @{
    double GetWallThickness() const {return fWallThickness;}
    void SetWallThickness(double v) {fWallThickness = v;}
    /// @}

    /// Get (or set) the depth of the liquid argon for the cryostat.  
    /// @{
    double GetArgonDepth() const {return fArgonDepth;}
    void SetArgonDepth(double v) {fArgonDepth = v;}
    /// @}

    /// Get (or set) distance from the bottom of the cryostat to the bottom of
    /// the drift region.  @{
    double GetBottomSpace() const {return fBottomSpace;}
    void SetBottomSpace(double v) {fBottomSpace = v;}
    /// @}

    /// Return the offset of the cryostat from the center of the world so that
    /// the center of the wire planes are at the origin.  The bottom of the V
    /// wire plane is at the origin.  This means that the wires for the V
    /// plane are at a (very) small z coordinate.
    G4ThreeVector GetOffset();

private:
    void Init(void);

    /// The diameter of the cold volume.
    double fInnerDiameter;

    /// The height of the cold volume.
    double fInnerHeight;

    /// The wall thickness for the cryostat.
    double fWallThickness;

    /// The liquid argon depth
    double fArgonDepth;

    /// The space between the bottom of the drift region and the cryostat wall.
    double fBottomSpace;

};
#endif
