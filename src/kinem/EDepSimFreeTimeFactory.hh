#ifndef EDepSim_FreeTimeFactory_hh_seen
#define EDepSim_FreeTimeFactory_hh_seen

#include "kinem/EDepSimVTimeFactory.hh"

namespace EDepSim {class FreeTimeFactory;}

/// Create a generator that leaves the time of the vertex free.  The vertex
/// time will either be zero, or will be set by the Kinematics generator.
class EDepSim::FreeTimeFactory : public EDepSim::VTimeFactory {
public:
    FreeTimeFactory(EDepSim::UserPrimaryGeneratorMessenger* parent);
    virtual ~FreeTimeFactory();

    /// Return the time generator.
    EDepSim::VTimeGenerator* GetGenerator();
};

#endif
