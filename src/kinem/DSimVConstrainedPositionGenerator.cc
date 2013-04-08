#include "kinem/DSimVConstrainedPositionGenerator.hh"
#include "DSimException.hh"

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

#include <queue>

DSimVConstrainedPositionGenerator::DSimVConstrainedPositionGenerator(
    const G4String& name) 
    : DSimVPositionGenerator(name), fSampleVolume("OA"),
      fLimitsFound(false) {}

DSimVConstrainedPositionGenerator::~DSimVConstrainedPositionGenerator() {}

bool DSimVConstrainedPositionGenerator::ForcePosition() {
    return true;
}

bool DSimVConstrainedPositionGenerator::ValidPosition(
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
        : public DSimVConstrainedPositionGenerator::PositionTest {
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
        : public DSimVConstrainedPositionGenerator::PositionTest {
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
        : public DSimVConstrainedPositionGenerator::PositionTest {
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
        : public DSimVConstrainedPositionGenerator::PositionTest {
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

};

void DSimVConstrainedPositionGenerator::CheckVolumeName(
    const G4String& name) {
    fPositionTests.push_back(new InternalVolumeName(name));
};

void DSimVConstrainedPositionGenerator::CheckNotVolumeName(
    const G4String& name) {
    fPositionTests.push_back(new InternalNotVolumeName(name));
};

void DSimVConstrainedPositionGenerator::CheckVolumeMaterial(
    const G4String& name) {
    fPositionTests.push_back(new InternalVolumeMaterial(name));
};

void DSimVConstrainedPositionGenerator::CheckNotVolumeMaterial(
    const G4String& name) {
    fPositionTests.push_back(new InternalNotVolumeMaterial(name));
};

void DSimVConstrainedPositionGenerator::CheckMinX(double x) {
    fPositionTests.push_back(new InternalMinimumCoordinate(0,x));
};

void DSimVConstrainedPositionGenerator::CheckMinY(double y) {
    fPositionTests.push_back(new InternalMinimumCoordinate(1,y));
};

void DSimVConstrainedPositionGenerator::CheckMinZ(double z) {
    fPositionTests.push_back(new InternalMinimumCoordinate(2,z));
};

void DSimVConstrainedPositionGenerator::CheckMaxX(double x) {
    fPositionTests.push_back(new InternalMaximumCoordinate(0,x));
};

void DSimVConstrainedPositionGenerator::CheckMaxY(double y) {
    fPositionTests.push_back(new InternalMaximumCoordinate(1,y));
};

void DSimVConstrainedPositionGenerator::CheckMaxZ(double z) {
    fPositionTests.push_back(new InternalMaximumCoordinate(2,z));
};

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
};

void DSimVConstrainedPositionGenerator::FindLimits() {
    if (fLimitsFound) return;
    fLimitsFound = true;

    if (fSampleVolume == "") {
        DSimError("DSimVConstrainedPositionGenerator:: "
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
            fMinimumCorner.set(volExtent.GetXmin(),
                               volExtent.GetYmin(),
                               volExtent.GetZmin());
            fMaximumCorner.set(volExtent.GetXmax(),
                               volExtent.GetYmax(),
                               volExtent.GetZmax());
            fMinimumCorner = rotation*fMinimumCorner + translation;
            fMaximumCorner = rotation*fMaximumCorner + translation;
            break;
        } 

        G4LogicalVolume* logVolume = phyVolume->GetLogicalVolume();
        for (int i=0; i< logVolume->GetNoDaughters(); ++i) {
            volumes.push(QueueElement(logVolume->GetDaughter(i),
                                      translation,
                                      rotation));
        }
    }

    CaptLog("Sampling volume extent:");
    CaptLog("    X: " << G4BestUnit(fMinimumCorner[0],"Length") 
             << "to " << G4BestUnit(fMaximumCorner[0],"Length"));
    CaptLog("    Y: " << G4BestUnit(fMinimumCorner[1],"Length") 
             << "to " << G4BestUnit(fMaximumCorner[1],"Length"));
    CaptLog("    Z: " << G4BestUnit(fMinimumCorner[2],"Length") 
             << "to " << G4BestUnit(fMaximumCorner[2],"Length"));
}

G4LorentzVector DSimVConstrainedPositionGenerator::TrialPosition() {
    FindLimits();
    G4LorentzVector vtx;
    for (int i=0; i<3; ++i) {
        vtx[i] = (fMaximumCorner[i] - fMinimumCorner[i])*G4UniformRand()
            + fMinimumCorner[i];
    }
    vtx[3] = 0;
    return vtx;
}
