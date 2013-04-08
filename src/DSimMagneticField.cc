#include "DSimMagneticField.hh"
#include "DSimException.hh"

#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>

#include <TCaptLog.hxx>

DSimMagneticField::DSimMagneticField(G4double f) 
    : fFieldStrength(f),
      fYokeOuterWidth(0.), fYokeOuterHeight(0.), fYokeOuterLength(0.),
      fYokeInnerWidth(0.), fYokeInnerHeight(0.), fYokeInnerLength(0.),
      fCoilOuterWidth(0.), fCoilOuterHeight(0.), fCoilOuterLength(0.),
      fCoilInnerWidth(0.), fCoilInnerHeight(0.), fCoilInnerLength(0.) {
    GetGlobalFieldManager()->SetDetectorField(this);
    GetGlobalFieldManager()->CreateChordFinder(this);
}

void DSimMagneticField::SetFieldStrength(G4double s) {

    fFieldStrength = s;

    // Find the Field Manager for the global field
    G4FieldManager* fieldMgr= GetGlobalFieldManager();

    if (fFieldStrength != 0.) {
       fieldMgr->SetDetectorField(this);
    } 
    else {
        // If the new field's value is zero, then it is best to
        // insure that it is not used for propagation.
        fieldMgr->SetDetectorField(NULL);
    }
}

void DSimMagneticField::GetFieldValue(const double point[4],
                                       double *Bfield ) const {
    G4ThreeVector pos(point[0], point[1], point[2]);

    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;

    if ((fYokeInnerWidth > fYokeOuterWidth)
        || (fYokeInnerHeight > fYokeOuterHeight)
        || (fYokeInnerLength > fYokeOuterLength)
        || (fCoilOuterWidth > fYokeInnerWidth)
        || (fCoilOuterHeight > fYokeInnerHeight)
        || (fCoilOuterLength > fYokeInnerLength)
        || (fCoilInnerWidth > fCoilOuterWidth)
        || (fCoilInnerHeight > fCoilOuterHeight)
        || (fCoilInnerLength > fCoilOuterLength)) {
        CaptLog("DSimMagneticField:: Bad coil and yoke dimensions");
        CaptLog("Outer Yoke Width:    " << fYokeOuterWidth);
        CaptLog("Inner Yoke Width:    " << fYokeInnerWidth);
        CaptLog("Outer Coil Width:    " << fCoilOuterWidth);
        CaptLog("Inner Coil Width:    " << fCoilInnerWidth);
        CaptLog("Outer Yoke Height:   " << fYokeOuterHeight);
        CaptLog("Inner Yoke Height:   " << fYokeInnerHeight);
        CaptLog("Outer Coil Height:   " << fCoilOuterHeight);
        CaptLog("Inner Coil Height:   " << fCoilInnerHeight);
        CaptLog("Outer Yoke Length:   " << fYokeOuterLength);
        CaptLog("Inner Yoke Length:   " << fYokeInnerLength);
        CaptLog("Outer Coil Length:   " << fCoilOuterLength);
        CaptLog("Inner Coil Length:   " << fCoilInnerLength);

        DSimError("DSimMagneticField:: Bad coil and yoke dimensions");
    }

    if (std::abs(pos.x()) > fYokeOuterWidth/2) return;
    if (std::abs(pos.y()) > fYokeOuterHeight/2) return;
    if (std::abs(pos.z()) > fYokeOuterLength/2) return;

    if (std::abs(pos.x()) < fCoilInnerWidth/2
        &&std::abs(pos.y()) < fCoilInnerHeight/2 
        && std::abs(pos.z()) < fCoilInnerLength/2) {
        // Inside the inner volume of the magnet.  This will eventually come
        // from a measured field map.
        Bfield[0] = fFieldStrength;
        Bfield[1] = 0.0;
        Bfield[2] = 0.0;
    }
    else if (std::abs(pos.x()) < fCoilOuterWidth/2
        &&std::abs(pos.y()) < fCoilOuterHeight/2 
        && std::abs(pos.z()) < fCoilOuterLength/2) {
        // Inside the coil volume of the magnet.
        double Y = (std::abs(pos.y()) - fCoilInnerHeight/2)
            / (fCoilOuterHeight/2 - fCoilInnerHeight/2);
        double Z = (std::abs(pos.z()) - fCoilInnerLength/2)
            / (fCoilOuterLength/2 - fCoilInnerLength/2);
        double depth = std::max(Y,Z);
        if (depth<0.0) depth = 0.0;
        if (depth>1.0) depth = 1.0;
        Bfield[0] = fFieldStrength * (1.0 - depth);
        Bfield[1] = 0.0;
        Bfield[2] = 0.0;
    }

#ifdef REST_OF_FIELD
    else if (std::abs(pos.x()) < fInnerVolumeWidth/2) {
        // In the top or bottom part of the flux return
        double innerArea = fInnerVolumeHeight*fInnerVolumeLength;
        double fluxArea = (fMagnetHeight-fInnerVolumeHeight)*fMagnetLength;
        double field = fFieldStrength*innerArea/fluxArea;
        Bfield[0] = -field;
        Bfield[1] = 0.0;
        Bfield[2] = 0.0;
    }        
    else if (std::abs(pos.y()) < fInnerVolumeHeight/2) {
        // In the side part of the flux return.
        double innerArea = fInnerVolumeHeight*fInnerVolumeLength;
        double fluxArea = (fMagnetHeight-fInnerVolumeHeight)*fMagnetLength;
        double field = fFieldStrength*innerArea/fluxArea;
        if (pos.x()<0) field = -field;
        Bfield[0] = 0.0;
        Bfield[1] = field*pos.y()*fInnerVolumeHeight;
        Bfield[2] = 0.0;
    }
    else {
        // In the corners of the flux return.
        double innerArea = fInnerVolumeHeight*fInnerVolumeLength;
        double fluxArea = (fMagnetHeight-fInnerVolumeHeight)*fMagnetLength;
        double field = fFieldStrength*innerArea/fluxArea/1.414;
        Bfield[0] = -field;
        Bfield[1] = field;
        if (pos.x() < 0.0) Bfield[1] = - Bfield[1];
        Bfield[2] = 0.0;
    }
#endif

    return;
}

G4FieldManager* DSimMagneticField::GetGlobalFieldManager()
{
  return G4TransportationManager::GetTransportationManager()->GetFieldManager();
}

