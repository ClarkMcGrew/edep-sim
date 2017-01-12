////////////////////////////////////////////////////////////
// $Id: EDepSim::VolumeId.cc,v 1.4 2010/04/07 18:06:51 gum Exp $
//

#include <vector>

#include <G4TouchableHandle.hh>

#include "EDepSimVolumeId.hh"

EDepSim::VolumeId::~VolumeId() {}

EDepSim::VolumeId::VolumeId() {}

EDepSim::VolumeId::VolumeId(const G4TouchableHandle& handle) {
    if(fVolumes.size()>0)fVolumes.clear();
    int historyDepth = handle->GetHistoryDepth();

    for (int i=0; i< historyDepth; ++i) {
        AddVolume(handle->GetVolume(i),handle->GetReplicaNumber(i));
    }

}

EDepSim::VolumeId& EDepSim::VolumeId::operator = (const G4TouchableHandle& handle) {
    if(fVolumes.size()>0)fVolumes.clear();
    int historyDepth = handle->GetHistoryDepth();

    for (int i=0; i< historyDepth; ++i) {
        AddVolume(handle->GetVolume(i),handle->GetReplicaNumber(i));
    }
    
    return *this;
}

EDepSim::VolumeId& EDepSim::VolumeId::operator = (const EDepSim::VolumeId& id) {
    if(fVolumes.size()>0)fVolumes.clear();
    for (EDepSim::VolumeId::Volumes::const_iterator i=id.fVolumes.begin();
         i != id.fVolumes.end();
         ++i) {
        AddVolume(i->fHandle, i->fReplica);
    }
    
    return *this;
}

void EDepSim::VolumeId::AddVolume(G4VPhysicalVolume* handle, int replica) {
    Volume vol;
    vol.fHandle = handle;
    vol.fReplica = replica;
    fVolumes.push_back(vol);
}

bool operator == (const EDepSim::VolumeId& x, const EDepSim::VolumeId& y) {
    if (x.fVolumes.size() != y.fVolumes.size()) return false;
    EDepSim::VolumeId::Volumes::const_iterator a;
    EDepSim::VolumeId::Volumes::const_iterator b;
    for (a = x.fVolumes.begin(), b = y.fVolumes.begin();
         a != x.fVolumes.end();
         ++a, ++b) {
        if (a->fHandle != b->fHandle) return false;
        if (a->fReplica != b->fReplica) return false;
    }
    return true;
}

bool operator != (const EDepSim::VolumeId& x, const EDepSim::VolumeId& y) {
    return !(x==y);
}

bool operator == (const EDepSim::VolumeId& x, const G4TouchableHandle& y) {
    unsigned int historyDepth = y->GetHistoryDepth();
    if (x.fVolumes.size() != historyDepth) return false;
    EDepSim::VolumeId::Volumes::const_iterator a;
    int b;
    for (a = x.fVolumes.begin(), b=0;
         a != x.fVolumes.end() && b < 4;
         ++a, ++b) {
        if (a->fHandle != y->GetVolume(b)) {
            return false;
        }
        if (a->fReplica != y->GetReplicaNumber(b)) {
            return false;
        }
    }
    return true;
}

bool operator == (const G4TouchableHandle& x, const EDepSim::VolumeId& y) {
    return (y==x);
}

bool operator != (const EDepSim::VolumeId& x, const G4TouchableHandle& y) {
    return !(x==y);
}

bool operator != (const G4TouchableHandle& x, const EDepSim::VolumeId& y) {
    return !(y==x);
}

bool operator < (const EDepSim::VolumeId& x, const EDepSim::VolumeId& y) {
    if (x.fVolumes.size() < y.fVolumes.size()) return true;
    if (x.fVolumes.size() > y.fVolumes.size()) return false;
    EDepSim::VolumeId::Volumes::const_iterator a;
    EDepSim::VolumeId::Volumes::const_iterator b;
    for (a = x.fVolumes.begin(), b = y.fVolumes.begin();
         a != x.fVolumes.end();
         ++a, ++b) {
        if (a->fHandle < b->fHandle) return true;
        if (a->fHandle > b->fHandle) return false;
        if (a->fHandle == b->fHandle) {
            if (a->fReplica < b->fReplica) return true;
            if (a->fReplica > b->fReplica) return false;
        }
    }
    return false;
}

std::ostream& operator << (std::ostream& stream, const EDepSim::VolumeId& v) {
    EDepSim::VolumeId::Volumes::const_iterator b;
    stream<< "<VolumeId ";
    if (v.fVolumes.size()<1) {
        stream << "empty>";
        return stream;
    }
    if (!v.fVolumes.front().fHandle) {
        stream << "null>";
        return stream;
    }
    stream << v.fVolumes.front().fHandle->GetName() << ">";
    return stream;
}

