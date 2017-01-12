#ifndef CaptDriftRegionBuilder_hh_Seen
#define CaptDriftRegionBuilder_hh_Seen

#include "EDepSimBuilder.hh"

class G4LogicalVolume;

/// Construct the drift region of the TPC.  The wire planes are located at the
/// top of the drift region, with the first wire for each plane located at the
/// most negative X coordinate.  The U plane runs from negative Y to positive
/// Y.  The V plane runs from positive Y to negative Y.  The planes are
/// oriented from top to bottom X, U, V (in otherwords, the first plane seen
/// by the electrons is the V plane).  The X plane is the collection plane.
class CaptDriftRegionBuilder : public EDepSim::Builder {
public:
    CaptDriftRegionBuilder(G4String name, EDepSim::Builder* parent)
        : EDepSim::Builder(name,parent) {Init();};
    virtual ~CaptDriftRegionBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Get the total height of the drift region.
    double GetHeight();

    /// Set the radius of the largest cylinder that can be contained in the
    /// drift region.
    /// @{
    void SetApothem(double v) {fApothem = v;}
    double GetApothem() {return fApothem;}
    /// @}

    /// Set the radius of the smallest cylinder that contains the drift
    /// region.  This is the maximum local Y dimension and is the apothem
    /// divided by the cosine of 30 degrees.
    /// @{
    void SetRadius(double v) {fApothem = v*0.866025403784;}
    double GetRadius() const {return fApothem/0.866025403784;}
    /// @}

    /// Set the length of the drift region from the cathode to the first wire
    /// plane.  (i.e. the grid plane).
    /// @{
    void SetDriftLength(double v) {fDriftLength = v;}
    double GetDriftLength() {return fDriftLength;}
    /// @}
    
    /// Set the spacing between the wire planes.
    void SetWirePlaneSpacing(double v) {fWirePlaneSpacing = v;}
    double GetWirePlaneSpacing() {return fWirePlaneSpacing;}
    /// @}

    /// Return the offset of the intended origin of the volume relative to the
    /// center of the logical volume.  To get the intended origin at the right
    /// location (say originPosition), the logical volume should be positioned
    /// at originPosition-GetOffset().  For the drift region, the intended
    /// origin is at the "center" of the bottom of the wire planes is at the
    /// global origin.
    G4ThreeVector GetOffset();
    
    /// Get the offset of the X plane from the top of the drift region.
    double GetXPlaneOffset();

    /// Get the offset of the X plane from the top of the drift region.
    double GetVPlaneOffset();

    /// Get the offset of the X plane from the top of the drift region.
    double GetUPlaneOffset();

    /// Get the offset of the grid from the top of the drift region.
    double GetGridPlaneOffset();

    /// Get the offset of the ground from the top of the drift region.  The
    /// distance between the grid and the ground defines the drift distance
    /// for the electrons.
    double GetGroundPlaneOffset();

private:
    void Init(void);

    /// The radius of the circle that can fit inside.
    double fApothem;

    /// The distance from the cathode to the first wire plane.
    double fDriftLength;

    /// The distance between the wire planes.
    double fWirePlaneSpacing;

};
#endif
