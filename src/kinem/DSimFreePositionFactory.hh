#ifndef DSimFreePositionFactory_hh_seen
#define DSimFreePositionFactory_hh_seen

#include "kinem/DSimVPositionFactory.hh"

class DSimFreePositionFactory : public DSimVPositionFactory {
public:
    DSimFreePositionFactory(DSimUserPrimaryGeneratorMessenger* parent);
    virtual ~DSimFreePositionFactory();

    /// Return the fixed vertex generator.
    DSimVPositionGenerator* GetGenerator();

};

#endif
