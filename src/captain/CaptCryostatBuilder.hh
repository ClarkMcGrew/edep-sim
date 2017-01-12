#ifndef CaptCryostatBuilder_hh_Seen
#define CaptCryostatBuilder_hh_Seen

#include "EDepSimBuilder.hh"
class G4LogicalVolume;

#include <string>

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
class CaptCryostatBuilder : public EDepSim::Builder {
public:
    CaptCryostatBuilder(G4String name, EDepSim::Builder* parent)
        : EDepSim::Builder(name,parent) {Init();};
    virtual ~CaptCryostatBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Get (or set) the distance from the flange to the top of the liquid
    /// argon.
    /// @{
    double GetArgonDepth() const {return fArgonDepth;}
    void SetArgonDepth(double v) {fArgonDepth = v;}
    /// @}

    /// Get (or set) the distance from the flange to the TPC center.
    /// @{
    double GetTPCDepth() const {return fTPCDepth;}
    void SetTPCDepth(double v) {fTPCDepth = v;}
    /// @}

    /// Get (or set) the type of vessel that should be built.
    /// @{
    std::string GetVesselType() const {return fVesselType;}
    void SetVesselType(std::string v) {fVesselType=v;}
    /// @}
    
    /// Return the offset of the intended origin of the volume relative to the
    /// center of the logical volume.  To get the origin at the right location
    /// (say originPosition), the logical volume should be positioned at
    /// originPosition-GetOffset().  The origin of the cryostat is chosen so
    /// that it's at the center of flange (the top of the "bucket" flange"
    /// defines the Z origin of the cryostat).  The bottom of the wire plane
    /// assembly is at the origin (a decision will be made in the future as to
    /// whether this is the bottom of the grid, or the bottom of the V plane.
    /// This means that the wires for the V plane are at a (very) small
    /// positive z coordinate.
    G4ThreeVector GetOffset();

    /// The position of the TPC origin (center of the grid plane defining the
    /// drift region) relative to the center of the Cryostat flange.  The TPC
    /// origin should be arranged so that it will be at center of the global
    /// coordinate system.
    G4ThreeVector GetTPCOffset();

private:
    void Init(void);

    /// The volume for the ullage.  This is set when the cryostat vessel is
    /// constructed and fills the gas part of the inner void.  It can be
    /// passed to daughter volumes to define the shape of the ullage.
    G4LogicalVolume* fUllageVolume;

    /// The volume for the liquid argon.  This is set when the cryostat vessel
    /// is constructed and fills the liquid part of the inner void.  It can be
    /// passed to the daughter volumes to define the shape of the liquid.
    G4LogicalVolume* fLiquidVolume;
    
    /// The name of the vessel to be built (currently CAPTAIN or mCAPTAIN).
    std::string fVesselType;

    /// The distance from the top of the flange to the liquid argon.
    double fArgonDepth;

    /// The distance from the top of the flange to the TPC.
    double fTPCDepth;
    
    class Point {
    public:
        Point(double z, double i, double o): fZ(z), fInner(i), fOuter(o) {}
        double fZ;
        double fInner;
        double fOuter;
    };
    typedef std::vector<Point> Shape;

    /// The inner vessel polycone points.
    Shape fInnerVessel;
    /// The outer vessel polycone points.
    Shape fOuterVessel;
    /// The vessel envelop.
    Shape fVesselEnvelope;

    /// Fill the vessel definition with values for CAPTAIN
    void DefineCAPTAINVessel();

    /// Fill the vessel definition with values for miniCAPTAIN
    void DefineMiniCAPTAINVessel();
    
};
#endif

