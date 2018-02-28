#ifndef TRecurseGeoManager_hxx_seen
#define TRecurseGeoManager_hxx_seen
#include <iostream>

#include <TGeoManager.h>
#include <TGeoNode.h>

class TRecurseGeoManager {
public:
    TRecurseGeoManager() {}
    virtual ~TRecurseGeoManager() {}

    /// Apply the action recursively to every node starting with the current
    /// node (depth first).
    bool Apply(int depth=0) {
        TGeoNode * node = gGeoManager->GetCurrentNode();
        if (!Action(depth)) {
            gGeoManager->CdUp();
            return false;
        }

        for (int i=0; i< node->GetNdaughters(); ++i) {
            gGeoManager->CdDown(i);
            if (!Apply(depth+1)) {
                gGeoManager->CdUp();
                return false;
            }
        }

        gGeoManager->CdUp();
        return true;
    }

    /// An action to override.  Return false if the recursion should be
    /// stopped.  The action should act on the current node, and NOT change
    /// the geometry.  The input argument (depth) is the current depth in the
    /// geometry (zero is the node where the recursion started).
    virtual bool Action(int depth) {
        TGeoNode* node = gGeoManager->GetCurrentNode();
        TGeoExtension* extension;
        extension = node->GetUserExtension();
        if (extension) {
            std::cout << "Have node user extension " << node->GetName()
                      << " depth " << depth
                      << std::endl;
        }
        extension = node->GetFWExtension();
        if (extension) {
            std::cout << "Have node FW extension " << node->GetName()
                      << " depth " << depth
                      << std::endl;
        }
        TGeoVolume* volume = node->GetVolume();
        extension = volume->GetUserExtension();
        if (extension) {
            std::cout << "Have volume user extension " << volume->GetName()
                      << " depth " << depth
                      << std::endl;
        }
        extension = volume->GetFWExtension();
        if (extension) {
            std::cout << "Have volume FW extension " << volume->GetName()
                      << " depth " << depth
                      << std::endl;
        }
        return true;
    }
};
#endif
