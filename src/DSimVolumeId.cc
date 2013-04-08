////////////////////////////////////////////////////////////
// $Id: DSimVolumeId.cc,v 1.4 2010/04/07 18:06:51 gum Exp $
//

#include <vector>

#include <G4TouchableHandle.hh>

#include "DSimVolumeId.hh"

DSimVolumeId::~DSimVolumeId() {}

DSimVolumeId::DSimVolumeId() {}

DSimVolumeId::DSimVolumeId(const G4TouchableHandle& handle) {
    if(fVolumes.size()>0)fVolumes.clear();
    int historyDepth = handle->GetHistoryDepth();

    for (int i=0; i< historyDepth; ++i) {
        AddVolume(handle->GetVolume(i),handle->GetReplicaNumber(i));
    }

}

DSimVolumeId& DSimVolumeId::operator = (const G4TouchableHandle& handle) {
    if(fVolumes.size()>0)fVolumes.clear();
    int historyDepth = handle->GetHistoryDepth();

    for (int i=0; i< historyDepth; ++i) {
        AddVolume(handle->GetVolume(i),handle->GetReplicaNumber(i));
    }
    
    return *this;
}

DSimVolumeId& DSimVolumeId::operator = (const DSimVolumeId& id) {
    if(fVolumes.size()>0)fVolumes.clear();
    for (DSimVolumeId::Volumes::const_iterator i=id.fVolumes.begin();
         i != id.fVolumes.end();
         ++i) {
        AddVolume(i->fHandle, i->fReplica);
    }
    
    return *this;
}

void DSimVolumeId::AddVolume(G4VPhysicalVolume* handle, int replica) {
    Volume vol;
    vol.fHandle = handle;
    vol.fReplica = replica;
    fVolumes.push_back(vol);
}

bool operator == (const DSimVolumeId& x, const DSimVolumeId& y) {
    if (x.fVolumes.size() != y.fVolumes.size()) return false;
    DSimVolumeId::Volumes::const_iterator a;
    DSimVolumeId::Volumes::const_iterator b;
    for (a = x.fVolumes.begin(), b = y.fVolumes.begin();
         a != x.fVolumes.end();
         ++a, ++b) {
        if (a->fHandle != b->fHandle) return false;
        if (a->fReplica != b->fReplica) return false;
    }
    return true;
}

bool operator == (const DSimVolumeId& x, const G4TouchableHandle& y) {
    unsigned int historyDepth = y->GetHistoryDepth();
    if (x.fVolumes.size() != historyDepth) return false;
    DSimVolumeId::Volumes::const_iterator a;
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

bool operator == (const G4TouchableHandle& x, const DSimVolumeId& y) {
    return (y==x);
}

bool operator != (const DSimVolumeId& x, const DSimVolumeId& y) {
    return !(x==y);
}

bool operator != (const DSimVolumeId& x, const G4TouchableHandle& y) {
    return !(x==y);
}

bool operator != (const G4TouchableHandle& x, const DSimVolumeId& y) {
    return !(y==x);
}

bool operator < (const DSimVolumeId& x, const DSimVolumeId& y) {
    if (x.fVolumes.size() < y.fVolumes.size()) return true;
    if (x.fVolumes.size() > y.fVolumes.size()) return false;
    DSimVolumeId::Volumes::const_iterator a;
    DSimVolumeId::Volumes::const_iterator b;
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

