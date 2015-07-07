#ifndef CaptWirePlaneBuilder_hh_Seen
#define CaptWirePlaneBuilder_hh_Seen

#include "DSimBuilder.hh"

class G4LogicalVolume;

/// Construct an unrotated wire plane.  In the local coordinate system, the
/// wires are oriented along the Y axis, and the wire number increases from
/// negative X to positive X.  The electric field points along the Z axis, or
/// in other words, into the drift region, and electrons drift from positive Z
/// toward the planes (i.e. in the negative Z direction).
/// 
/// The wires are not "tubes", but are boxes that represent the area overwhich
/// the wire will measure charge.  The name of the wire planes is significant
/// since captevent will assigned a geometry identifier based on the name
/// "XPlane", "UPlane", "VPlane".  There can only be one with each name.
///
class CaptWirePlaneBuilder : public DSimBuilder {
public:
    CaptWirePlaneBuilder(G4String name, DSimBuilder* parent)
        : DSimBuilder(name,parent) {Init();};
    virtual ~CaptWirePlaneBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Get the total height (thickness) of the drift region.
    /// @{
    void SetHeight(double v) {fHeight = v;}
    double GetHeight() const {return fHeight;}
    /// @}

    /// Set the radius of the largest cylinder that can be contained in the
    /// hexagonal wire plane.  This is the maximum local X dimension. 
    /// @{
    void SetApothem(double v) {fApothem = v;}
    double GetApothem() const {return fApothem;}
    /// @}

    /// Set the radius of the smallest cylinder that contains the hexagonal
    /// wire plane.  This is the maximum local Y dimension and is the apothem
    /// divided by the cosine of 30 degrees.
    /// @{
    void SetRadius(double v) {fApothem = v*0.866025403784;}
    double GetRadius() const {return fApothem/0.866025403784;}
    /// @}

    /// Get or set the distance between wires.  
    /// @{
    void SetSpacing(double v) {fSpacing = v;}
    double GetSpacing() const {return fSpacing;}
    /// @}

    /// Get or set the maximum number of wires.  The default number of wires
    /// is calculated based on the apothem and the wire spacing, but can be
    /// limited by SetMaxWireCount().
    /// @{
    void SetMaxWireCount(int v) {fMaxWireCount = v;}
    int GetMaxWireCount() const {return fMaxWireCount;}
    /// @}

private:
    void Init(void);

    /// The radius of the circle that can fit inside the wire plane
    double fApothem;

    /// The thickness of the wire plane.
    double fHeight;

    /// The wire to wire spacing.
    double fSpacing;

    /// The maximum number of wires in the plane. 
    int fMaxWireCount;
};
#endif
