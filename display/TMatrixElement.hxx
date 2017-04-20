#ifndef TMatrixElement_hxx_seen
#define TMatrixElement_hxx_seen

#include <TEveGeoShape.h>
#include <TMatrixD.h>
#include <TMatrixF.h>
#include <TVector3.h>

namespace EDep {
    class TMatrixElement;
};

/// A Eve Element object to represent a 3x3 matrix (either the moments of a
/// distribution, or the covariance) at a position.  This can be used to
/// represent clusters, or other objects, and handles some of the matrix
/// algebra necessary.  Since Eve can't draw a spheriod, the spheriod
/// described by the matrix is drawn as an eliptical tube.  If the longAxis
/// variable is true, then the longest axis of the spheriod is used as the
/// axis of the tube, but if it's false, the shortest axis of the spheriod is
/// used as the axis of the tube.
class EDep::TMatrixElement: public TEveGeoShape {
public:
    TMatrixElement(const char* name,
                   const TVector3& position,
                   const TMatrixD& matrix,
                   bool longAxis);
    TMatrixElement(const char* name,
                   const TVector3& position,
                   const TMatrixF& matrix,
                   bool longAxis);
    virtual ~TMatrixElement();

private:
    void Initialize(const TVector3& position,
                    const TMatrixD& matrix,
                    bool longAxis);
};
#endif
