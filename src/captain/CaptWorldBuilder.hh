#ifndef CaptWorldBuilder_hh_Seen
#define CaptWorldBuilder_hh_Seen

#include "DSimBuilder.hh"

class G4LogicalVolume;

/// Construct the world volume.  The origin is located at the center of the
/// detector coordinate system.  The world is mostly filled with air.
class CaptWorldBuilder : public DSimBuilder {
public:
    CaptWorldBuilder(G4String n, DSimUserDetectorConstruction* c)
        : DSimBuilder(n,c) {Init();};
    virtual ~CaptWorldBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void);

    /// Set the length of the world
    void SetLength(double v) {fLength = v;}

    /// Set the width of the world
    void SetWidth(double v) {fWidth = v;}
    
    /// Set the height of the world
    void SetHeight(double v) {fHeight = v;}
    
    /// Get the length of the world
    double GetLength() {return fLength;}
    
    /// Get the width of the world
    double GetWidth() {return fWidth;}
    
    /// Get the height of the world
    double GetHeight() {return fHeight;}
    
    
private:
    void Init(void);

    /// The distance along the X axis to simulate. 
    double fLength;

    /// The side to side (Y) dimension to be simulated
    double fWidth;

    /// The height (Z) of the world coordinate system. 
    double fHeight;

};
#endif
