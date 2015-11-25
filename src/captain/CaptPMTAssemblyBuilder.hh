#ifndef CaptPMTAssemblyBuilder_hh_Seen
#define CaptPMTAssemblyBuilder_hh_Seen

#include "DSimBuilder.hh"

class G4LogicalVolume;

/// Construct the PMT assembly for the CAPTAIN detector.  The location of the
/// PMT Assembly is relative to the "top" surface, and the PMTs face "up".
/// The assembly is a half hexagon.
class CaptPMTAssemblyBuilder : public DSimBuilder {
public:
    CaptPMTAssemblyBuilder(G4String name, DSimBuilder* parent)
        : DSimBuilder(name,parent) {Init();};
    virtual ~CaptPMTAssemblyBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Get the total height of the pmt assembly.
    double GetHeight();

    /// Set the radius of the largest cylinder that can be contained in the
    /// pmt region.
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

    /// Set the thickness of the mounting plate.
    /// {@
    void SetPlateThickness(double v) {fPlateThickness = v;}
    double GetPlateThickness() const {return fPlateThickness;}
    /// @}

    /// Set the radius of the holes in the mounting plate.
    /// {@
    void SetHoleRadius(double v) {fHoleRadius = v;}
    double GetHoleRadius() const {return fHoleRadius;}
    /// @}
    
    /// Set the thickness of the TPB plates.
    /// {@
    void SetTPBThickness(double v) {fTPBThickness = v;}
    double GetTPBThickness() const {return fTPBThickness;}
    /// @}
    
    /// Set the size of the TPB plates.
    /// {@
    void SetTPBSize(double v) {fTPBSize = v;}
    double GetTPBSize() const {return fTPBSize;}
    /// @}
    
    /// Return the offset of the intended origin of the volume relative to the
    /// center of the logical volume.  To get the intended origin at the right
    /// location (say originPosition), the logical volume should be positioned
    /// at originPosition-GetOffset().  For the drift region, the intended
    /// origin is at the "center" of the bottom of the wire planes is at the
    /// global origin.
    G4ThreeVector GetOffset();
    
private:
    void Init(void);

    /// The radius of the circle that can fit inside.
    double fApothem;

    /// The size of the TPB plates.
    double fTPBSize;
    
    /// The thickness of the TPB plates.
    double fTPBThickness;
    
    /// The mounting plate thickness.
    double fPlateThickness;

    /// The radius of the holes in the mounting plate
    double fHoleRadius;
};
#endif
