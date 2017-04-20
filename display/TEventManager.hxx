#ifndef TEventManager_hxx_seen
#include <TG4Event.h>
#include <TTree.h>

namespace EDep {
    /// The tree containing the event.
    extern TTree* gEDepSimTree;

    /// The event being displayed.
    extern TG4Event *gEDepSimEvent;

    extern int gEDepSimEntryNumber;
}
#endif
