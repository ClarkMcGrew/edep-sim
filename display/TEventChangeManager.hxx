#ifndef TEventChangeManager_hxx_seen
#define TEventChangeManager_hxx_seen

#include <TFile.h>
#include <TObject.h>

namespace EDep {
    class TEventChangeManager;
    class TVEventChangeHandler;

}

/// A class to handle a new event becoming available to the event display.
/// There is a single instance of this class owned by TEventDisplay.  This
/// must be created after the GUI has been initialized.
class EDep::TEventChangeManager: public TObject {
public:
    TEventChangeManager();
    virtual ~TEventChangeManager();

    /// Set or get the event source.  When the event source is set, the first
    /// event is read.  @{
    void SetEventSource(TFile* source);
    TFile* GetEventSource() {return fEventSource;}
    /// @}

    /// Trigger an event change.  The argument says how many events to read.
    /// If it's positive, then it reads forward in the file.  If the change is
    /// zero, then the current event is just redrawn.  This is connected to
    /// the GUI buttons.
    void ChangeEvent(int change=1);

    /// Add a handler (taking ownership of the handler) for when the event
    /// changes (e.g. a new event is read).  These handlers are for
    /// "once-per-event" actions and are executed by the NewEvent() method.
    void AddNewEventHandler(EDep::TVEventChangeHandler* handler);
    
    /// Add a handler (taking ownership of the handler) for when the event
    /// needs to be redrawn.  These handlers are executed by the UpdateEvent()
    /// method.
    void AddUpdateHandler(EDep::TVEventChangeHandler* handler);
    
    /// Set the flag to show (or not show) the geometry
    void SetShowGeometry(bool f) {fShowGeometry = f;}
    bool GetShowGeometry() const {return fShowGeometry;}

    /// Add the name of a volume to be drawn.
    void AddVolumeToShow(const std::string& name) {
        fVolumesToShow.push_back(name);
    }

    /// Clear the list of volumes to be shown.
    void ClearVolumesToShow() {
        fVolumesToShow.clear();
    }

private:

    /// This updates the event display for a new event using the event change
    /// handlers.  It is used to do once-per-event initializations and does
    /// not call UpdateEvent().
    void NewEvent();

    /// This updates the event display for an event using the event change
    /// handlers.
    void UpdateEvent();

    /// The input source of events.
    TFile* fEventSource;

    typedef std::vector<EDep::TVEventChangeHandler*> Handlers;

    /// The event update handlers.
    Handlers fUpdateHandlers;

    /// The new event handlers.
    Handlers fNewEventHandlers;

    /// Flag to determine if the geometry will be drawn.
    bool fShowGeometry;

    /// A collection of volume names to be shown if the geometry is being drawn.
    std::vector<std::string> fVolumesToShow;
    
    ClassDef(TEventChangeManager,0);
};

#endif
