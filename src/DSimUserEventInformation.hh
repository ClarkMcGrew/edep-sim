////////////////////////////////////////////////////////
// $Id: DSimUserEventInformation.hh,v 1.5 2008/01/14 17:01:57 mcgrew Exp $

#ifndef DSimUserEventInformation_hh_seen
#define DSimUserEventInformation_hh_seen

#include <G4VUserEventInformation.hh>
#include <G4PrimaryVertex.hh>

class DSimUserEventInformation : public G4VUserEventInformation {
public:
    DSimUserEventInformation();
    virtual ~DSimUserEventInformation();
    
    /// Print information about the event.
    virtual void Print() const;

    /// Reset for a new event.  This is needed since some event generators
    /// start creating an event, and then later decide to reject it.  This
    /// allows the user information to be reset to the default values.
    void InitializeEvent(void);

private:
};
#endif
