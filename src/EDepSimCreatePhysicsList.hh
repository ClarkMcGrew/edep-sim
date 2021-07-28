#ifndef EDepSim_CreatePhysicsList_hh_seen
/////////////////////////////////////////////////////////////////////
// $Id: EDepSimCreatePhysicsList.hh,v 1.1 2021/05/26 00:39:00 rhatcher Exp $

#include <G4VModularPhysicsList.hh>

namespace EDepSim {
  class  CreatePhysicsListMessenger;

  G4VModularPhysicsList* CreatePhysicsList(G4String physicsList);
}

// access to the created messenger (read as CreatePhysicsList...Messenger)
extern EDepSim::CreatePhysicsListMessenger* gCreatePhysicsListMessenger;

#endif
