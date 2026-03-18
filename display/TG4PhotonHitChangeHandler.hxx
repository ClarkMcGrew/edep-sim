#ifndef TG4PhotonHitChangeHandler_hxx_seen
#define TG4PhotonHitChangeHandler_hxx_seen

#include "TVEventChangeHandler.hxx"

namespace EDep {
    class TG4PhotonHitChangeHandler;
}

class TEveElementList;

/// Handle drawing all optical-photon endpoints stored in TG4Event::PhotonHits.
///
/// Each photon is drawn as a small marker at its final position.
/// Photons that reached a sensor (CopyNo >= 0) are drawn in yellow;
/// photons that died elsewhere (CopyNo == -1) are drawn in blue.
class EDep::TG4PhotonHitChangeHandler: public TVEventChangeHandler {
public:
    TG4PhotonHitChangeHandler();
    ~TG4PhotonHitChangeHandler();

    virtual void Apply();

private:
    TEveElementList* fPhotonList;
};

#endif
