///////////////////////////////////////////////////////////////
// $Id: EDepSim::VolumeId.hh,v 1.2 2004/11/18 01:30:40 jnurep Exp $
//
#ifndef EDepSim_VolumeId_hh_seen
#define EDepSim_VolumeId_hh_seen

#include <vector>
#include <ostream>

#include <G4VPhysicalVolume.hh>
#include <G4TouchableHandle.hh>

namespace EDepSim {class VolumeId;}

bool operator ==(const EDepSim::VolumeId& x, const EDepSim::VolumeId& y);
bool operator ==(const EDepSim::VolumeId& x, const G4TouchableHandle& y);
bool operator ==(const G4TouchableHandle& x, const EDepSim::VolumeId& y);
bool operator !=(const EDepSim::VolumeId& x, const EDepSim::VolumeId& y);
bool operator !=(const EDepSim::VolumeId& x, const G4TouchableHandle& y);
bool operator !=(const G4TouchableHandle& x, const EDepSim::VolumeId& y);
bool operator <(const EDepSim::VolumeId& x,  const EDepSim::VolumeId& y);

std::ostream& operator << (std::ostream& stream, const EDepSim::VolumeId& v);

/// A class to provide a unique identifier for all physical volumes.  One of
/// the features of G4 is that physical volumes are identified by their
/// position in the volume tree.  This has many advantages, but it means that
/// it can be quite trick to figure out if two physical volume pointers refer
/// to the same physical volume, or different replicas of the same physical
/// volume.  (Consider the case where a you have an unreplicated sub-volume of
/// a replicated parent).  This class provides a unique comparable object that
/// allows an equality test between volumes taking into account the full
/// position in the hierarchy.  It is similar in function to
/// G4TouchableHandle, but provides better comparison operators.
class EDepSim::VolumeId {
public:
    /// Construct a new volume Id.
    VolumeId(const G4TouchableHandle& handle);
    /// Construct an empty volume Id.
    VolumeId();
    ~VolumeId();

    /// Explicitly add a new volume to the volume Id.
    void AddVolume(G4VPhysicalVolume* fHandle,int fReplica);

    bool operator !() {
        return (fVolumes.size()==0);
    };

    EDepSim::VolumeId& operator = (const G4TouchableHandle& handle);
    EDepSim::VolumeId& operator = (const EDepSim::VolumeId& id);

    friend bool ::operator ==(const EDepSim::VolumeId& x, 
                              const G4TouchableHandle& y);
    friend bool ::operator ==(const G4TouchableHandle& x,
                              const EDepSim::VolumeId& y);
    friend bool ::operator ==(const EDepSim::VolumeId& x,
                              const EDepSim::VolumeId& y);

    friend bool ::operator !=(const EDepSim::VolumeId& x,
                              const G4TouchableHandle& y);
    friend bool ::operator !=(const G4TouchableHandle& x,
                            const EDepSim::VolumeId& y);
    friend bool ::operator !=(const EDepSim::VolumeId& x,
                            const EDepSim::VolumeId& y);

    friend bool ::operator <(const EDepSim::VolumeId& x,
                             const EDepSim::VolumeId& y);

    friend std::ostream& ::operator << (std::ostream& s, const EDepSim::VolumeId&);
    
    class Volume {
    public:
        G4VPhysicalVolume* fHandle;
        int fReplica;
    };
    typedef std::vector<Volume> Volumes;

    /// This is a vector of information required to uniquely identify the
    /// volume.  It is arranged so that the most local information is at the
    /// front of the vector and the world volume will be the last element in
    /// the vector.
    Volumes fVolumes;
};
#endif
