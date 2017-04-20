#ifndef TVEventChangeHandler_hxx_seen
#define TVEventChangeHandler_hxx_seen

#include <TObject.h>

namespace EDep {
    class TVEventChangeHandler;
}

/// A base class for handlers called by TEventChangeManager.  The
/// TEventChangeManager keeps a vector of possible handlers that are used
/// everytime the event has changed (or needs to be reset).  The handlers need
/// to implement the XXX class, and should check to see if they are enabled
/// using the GUI class.
class EDep::TVEventChangeHandler: public TObject {
public:
    TVEventChangeHandler() {}
    virtual ~TVEventChangeHandler() {}

    /// Apply the change handler to the current event.  This does all of the
    /// work.
    virtual void Apply() = 0;
};
#endif
