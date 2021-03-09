////////////////////////////////////////////////////////////
// $Id: EDepSim::UserPrimaryGeneratorAction.hh,v 1.7 2012/05/10 16:27:26 mcgrew Exp $
//

#ifndef EDepSim_PrimaryGeneratorAction_hh
#define EDepSim_PrimaryGeneratorAction_hh 1

#include <map>
#include <vector>

#include "G4VUserPrimaryGeneratorAction.hh"

namespace EDepSim {class UserPrimaryGeneratorMessenger;}
class G4VPrimaryGenerator;

namespace EDepSim {class UserPrimaryGeneratorAction;}
class EDepSim::UserPrimaryGeneratorAction
    : public G4VUserPrimaryGeneratorAction {

public:
    UserPrimaryGeneratorAction(void);
    virtual ~UserPrimaryGeneratorAction();

    /// Generate all of the primaries for and event and place them into a
    /// primary vertex.  This may generate several primary vertices (in no
    /// particular order).
    virtual void GeneratePrimaries(G4Event* anEvent);

    /// Add a new EDepSim::PrimaryGenerator to the list of generators adding
    /// primary vertices to the event.
    void AddGenerator(G4VPrimaryGenerator* generator) {
        fPrimaryGenerators.push_back(generator);
    }

    /// Clear the current list of generators.
    void ClearGenerators();

    /// Get the generators used to create events.  There will be at least one.
    const G4VPrimaryGenerator* GetGenerator(int i) const {
        return fPrimaryGenerators[i];
    }

    /// Get the number of generators being used to create events.
    int GetGeneratorCount() const {
        return fPrimaryGenerators.size();
    }

    /// Set a flag that events that don't contain a real vertex are allowed.
    /// If the flag is true, then empty events are allowed.
    void SetAllowEmptyEvents(bool flag) {fAllowEmptyEvents = flag;}

    /// Set a flag that events that it's to run out of events in the
    /// kinematics file.  If the flag is true, then partial events are
    /// allowed.
    void SetAllowPartialEvents(bool flag) {fAllowPartialEvents = flag;}

    /// Set a flag that events without a real vertex should have a fake
    /// GEANTINO generated.  This only has an effect with
    /// SetAllowEmptyEvents() has been called with true.
    void SetAddFakeGeantino(bool flag) {fAddFakeGeantino = flag;}

private:

    /// A vector of generator sets to use to generate events.  Each of these
    /// primary factories will be called in sequence.
    std::vector<G4VPrimaryGenerator*> fPrimaryGenerators;

    /// A flag that empty events are allowed.  If this is set to be true, the
    /// generator will return events even if there are no real vertices.
    bool fAllowEmptyEvents;

    /// A flag that any empty event should be filled with a fake GEANTINO
    /// vertex.  This only has an affect with fAllowEmptyEvents is true.
    bool fAddFakeGeantino;

    /// A flag to allow an event to be generated even if the input kinematics
    /// has run out of data.  This is useful when a single spill of
    /// interactions has been generated and you want to generate a single
    /// event containing every interaction in the kinematic input file.
    bool fAllowPartialEvents;

    /// The messenger for this action
    EDepSim::UserPrimaryGeneratorMessenger* fMessenger;
};
#endif
