#ifndef CaptWirePlaneBuilder_hh_Seen
#define CaptWirePlaneBuilder_hh_Seen

#include "DSimBuilder.hh"

class G4LogicalVolume;

/// Construct an unrotated wire plane.  In the local coordinate system, the
/// wires are oriented along the Y axis, and the wire number increases from
/// negative X to positive X.  The electric field points along the Z axis, or
/// in other words, into the drift region.
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
    double GetHeight() {return fHeight;}
    /// @}

    /// Set the radius of the largest circle that can be contained in the
    /// drift region.  This is the maximum local Z dimension.
    /// @{
    void SetApothem(double v) {fApothem = v;}
    double GetApothem() {return fApothem;}
    /// @}

    /// Get or set the wire spacing.
    /// @{
    void SetSpacing(double v) {fSpacing = v;}
    double GetSpacing() {return fSpacing;}
    /// @}

private:
    void Init(void);

    /// The radius of the circle that can fit inside the wire plane
    double fApothem;

    /// The thickness of the wire plane.
    double fHeight;

    /// The wire to wire spacing.
    double fSpacing;
};
#endif
