#include "kinem/EDepSimVConstrainedPositionGenerator.hh"
#include "EDepSimException.hh"

#include <G4UnitsTable.hh>
#include <G4TransportationManager.hh>
#include <G4Navigator.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisExtent.hh>
#include <Randomize.hh>
#include <G4SystemOfUnits.hh>

#include <queue>

EDepSim::VConstrainedPositionGenerator::VConstrainedPositionGenerator(
    const G4String& name)
    : EDepSim::VPositionGenerator(name), fSampleVolume("Cryostat"),
      fLimitsFound(false) {
    fMaximumCorner.set(1000000*meter,1000000*meter,1000000*meter);
    fMinimumCorner.set(-1000000*meter,-1000000*meter,-1000000*meter);
}

EDepSim::VConstrainedPositionGenerator::~VConstrainedPositionGenerator() {}

bool EDepSim::VConstrainedPositionGenerator::ForcePosition() {
    return true;
}

bool EDepSim::VConstrainedPositionGenerator::ValidPosition(
    const G4LorentzVector& vtx) {
    for (PositionTests::iterator test = fPositionTests.begin();
         test != fPositionTests.end();
         ++test) {
        if (!(*test)->Apply(vtx)) return false;
    }
    return true;
}

namespace {
    // Check that the vertex is in a volume of a particular name.
    class InternalVolumeName
        : public EDepSim::VConstrainedPositionGenerator::PositionTest {
    public:
        InternalVolumeName(const G4String& name): fName(name) {};
        virtual ~InternalVolumeName() {};
        virtual bool Apply(const G4LorentzVector& vtx) {
            // Get the navigator.
            G4Navigator* navigator
                = G4TransportationManager::GetTransportationManager()
                ->GetNavigatorForTracking();

            // Get the volume that contains the point.
            G4VPhysicalVolume* volume
                = navigator->LocateGlobalPointAndSetup(vtx);

            if (!volume) return false;

            // Check that the point is inside the named volume.
            if (!volume->GetName().contains(fName)) {
                return false;
            }
            return true;
        }
    private:
        G4String fName;
    };

    // Check that the vertex is NOT in a volume of a particular name.
    class InternalNotVolumeName
        : public InternalVolumeName {
    public:
        InternalNotVolumeName(const G4String& name)
            : InternalVolumeName(name) {};
        virtual ~InternalNotVolumeName() {};
        virtual bool Apply(const G4LorentzVector& vtx) {
            return !InternalVolumeName::Apply(vtx);
        }
    };

    // Check that the vertex is in a volume of a particular material.
    class InternalVolumeMaterial
        : public EDepSim::VConstrainedPositionGenerator::PositionTest {
    public:
        InternalVolumeMaterial(const G4String& name): fMater(name) {};
        virtual ~InternalVolumeMaterial() {};
        virtual bool Apply(const G4LorentzVector& vtx) {
            // Get the navigator.
            G4Navigator* navigator
                = G4TransportationManager::GetTransportationManager()
                ->GetNavigatorForTracking();

            // Get the volume that contains the point.
            G4VPhysicalVolume* volume
                = navigator->LocateGlobalPointAndSetup(vtx);

            if (!volume) return false;

            G4String matter = volume->GetLogicalVolume()
                ->GetMaterial()->GetName();

            // Check that the point is inside the named material.
            if (!matter.contains(fMater)) return false;
            return true;
        }
    private:
        G4String fMater;
    };

    // Check that the vertex is NOT in a volume of a particular material.
    class InternalNotVolumeMaterial
        : public InternalVolumeMaterial {
    public:
        InternalNotVolumeMaterial(const G4String& name)
            : InternalVolumeMaterial(name) {};
        virtual ~InternalNotVolumeMaterial() {};
        virtual bool Apply(const G4LorentzVector& vtx) {
            return !InternalVolumeMaterial::Apply(vtx);
        }
    };

    // Check that the coordinate is greater than a value.
    class InternalMinimumCoordinate
        : public EDepSim::VConstrainedPositionGenerator::PositionTest {
    public:
        InternalMinimumCoordinate(int coord, double minimum):
            fCoordinate(coord), fValue(minimum) {};
        virtual ~InternalMinimumCoordinate() {};
        virtual bool Apply(const G4LorentzVector& vtx) {
            if (vtx[fCoordinate] < fValue) return false;
            return true;
        }
    private:
        int fCoordinate;
        double fValue;
    };

    // Check that the coordinate is less than a value.
    class InternalMaximumCoordinate
        : public EDepSim::VConstrainedPositionGenerator::PositionTest {
    public:
        InternalMaximumCoordinate(int coord, double maximum):
            fCoordinate(coord), fValue(maximum) {};
        virtual ~InternalMaximumCoordinate() {};
        virtual bool Apply(const G4LorentzVector& vtx) {
            if (fValue < vtx[fCoordinate]) return false;
            return true;
        }
    private:
        int fCoordinate;
        double fValue;
    };

}

void EDepSim::VConstrainedPositionGenerator::CheckVolumeName(
    const G4String& name) {
    fPositionTests.push_back(new InternalVolumeName(name));
}

void EDepSim::VConstrainedPositionGenerator::CheckNotVolumeName(
    const G4String& name) {
    fPositionTests.push_back(new InternalNotVolumeName(name));
}

void EDepSim::VConstrainedPositionGenerator::CheckVolumeMaterial(
    const G4String& name) {
    fPositionTests.push_back(new InternalVolumeMaterial(name));
}

void EDepSim::VConstrainedPositionGenerator::CheckNotVolumeMaterial(
    const G4String& name) {
    fPositionTests.push_back(new InternalNotVolumeMaterial(name));
}

void EDepSim::VConstrainedPositionGenerator::CheckMinX(double x) {
    if (fMinimumCorner.x() < x) fMinimumCorner.setX(x);
    fPositionTests.push_back(new InternalMinimumCoordinate(0,x));
}

void EDepSim::VConstrainedPositionGenerator::CheckMinY(double y) {
    if (fMinimumCorner.y() < y) fMinimumCorner.setY(y);
    fPositionTests.push_back(new InternalMinimumCoordinate(1,y));
}

void EDepSim::VConstrainedPositionGenerator::CheckMinZ(double z) {
    if (fMinimumCorner.z() < z) fMinimumCorner.setZ(z);
    fPositionTests.push_back(new InternalMinimumCoordinate(2,z));
}

void EDepSim::VConstrainedPositionGenerator::CheckMaxX(double x) {
    if (fMaximumCorner.x() > x) fMaximumCorner.setX(x);
    fPositionTests.push_back(new InternalMaximumCoordinate(0,x));
}

void EDepSim::VConstrainedPositionGenerator::CheckMaxY(double y) {
    if (fMaximumCorner.y() > y) fMaximumCorner.setY(y);
    fPositionTests.push_back(new InternalMaximumCoordinate(1,y));
}

void EDepSim::VConstrainedPositionGenerator::CheckMaxZ(double z) {
    if (fMaximumCorner.z() > z) fMaximumCorner.setZ(z);
    fPositionTests.push_back(new InternalMaximumCoordinate(2,z));
}

namespace {
    class QueueElement {
    public:
        QueueElement(G4VPhysicalVolume* v,
                     const G4ThreeVector& t,
                     const G4RotationMatrix& r)
            : volume(v), translation(t), rotation(r) {}
        G4VPhysicalVolume* volume;
        G4ThreeVector translation;
        G4RotationMatrix rotation;
    };
}

void EDepSim::VConstrainedPositionGenerator::FindLimits() {
    if (fLimitsFound) return;
    fLimitsFound = true;

    if (fSampleVolume == "") {
        EDepSimError("EDepSim::VConstrainedPositionGenerator:: "
                    "The sample volume must be set");
    }

    // Find the volume for sampling by something that is big enough to cover
    // all of the possible vertex locations.
    G4Navigator* navigator
        = G4TransportationManager::GetTransportationManager()
        ->GetNavigatorForTracking();

    std::queue<QueueElement> volumes;
    volumes.push(QueueElement(navigator->GetWorldVolume(),
                              G4ThreeVector(0,0,0),
                              G4RotationMatrix()));
    while (!volumes.empty()) {
        G4VPhysicalVolume* phyVolume = volumes.front().volume;
        G4ThreeVector translation
            = volumes.front().translation + phyVolume->GetObjectTranslation();
        G4RotationMatrix rotation = volumes.front().rotation;
        rotation.transform(phyVolume->GetObjectRotationValue());
        volumes.pop();

        if (phyVolume->GetName().find(fSampleVolume) != std::string::npos) {
            G4VisExtent volExtent
                = phyVolume->GetLogicalVolume()->GetSolid()->GetExtent();
            G4ThreeVector volMin;
            G4ThreeVector volMax;
            volMin.set(volExtent.GetXmin(),
                       volExtent.GetYmin(),
                       volExtent.GetZmin());
            volMax.set(volExtent.GetXmax(),
                               volExtent.GetYmax(),
                               volExtent.GetZmax());
            volMin = rotation*volMin + translation;
            volMax = rotation*volMax + translation;
            if (fMaximumCorner.x()>volMax.x()) fMaximumCorner.setX(volMax.x());
            if (fMaximumCorner.y()>volMax.y()) fMaximumCorner.setY(volMax.y());
            if (fMaximumCorner.z()>volMax.z()) fMaximumCorner.setZ(volMax.z());
            if (fMinimumCorner.x()<volMin.x()) fMinimumCorner.setX(volMin.x());
            if (fMinimumCorner.y()<volMin.y()) fMinimumCorner.setY(volMin.y());
            if (fMinimumCorner.z()<volMin.z()) fMinimumCorner.setZ(volMin.z());
            break;
        }

        G4LogicalVolume* logVolume = phyVolume->GetLogicalVolume();
        for (std::size_t i=0; i<(std::size_t)logVolume->GetNoDaughters(); ++i) {
            volumes.push(QueueElement(logVolume->GetDaughter(i),
                                      translation,
                                      rotation));
        }
    }

    EDepSimLog("Sampling volume extent:");
    EDepSimLog("    X: " << G4BestUnit(fMinimumCorner[0],"Length")
             << "to " << G4BestUnit(fMaximumCorner[0],"Length"));
    EDepSimLog("    Y: " << G4BestUnit(fMinimumCorner[1],"Length")
             << "to " << G4BestUnit(fMaximumCorner[1],"Length"));
    EDepSimLog("    Z: " << G4BestUnit(fMinimumCorner[2],"Length")
             << "to " << G4BestUnit(fMaximumCorner[2],"Length"));
}

G4LorentzVector EDepSim::VConstrainedPositionGenerator::TrialPosition() {
    FindLimits();
    G4LorentzVector vtx;
    for (int i=0; i<3; ++i) {
        vtx[i] = (fMaximumCorner[i] - fMinimumCorner[i])*G4UniformRand()
            + fMinimumCorner[i];
    }
    vtx[3] = 0;
    return vtx;
}
