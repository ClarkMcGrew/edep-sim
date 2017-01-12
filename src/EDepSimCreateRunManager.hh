#ifndef EDepSim_CreateRunManager_hh_seen
/////////////////////////////////////////////////////////////////////
// $Id: EDepSimCreateRunManager.hh,v 1.2 2009/04/09 00:39:00 gum Exp $

#include <G4RunManager.hh>
#include <G4VUserPhysicsList.hh>

/// Create the standard run manager for the detSim detector simulation.  The
/// caller is responsible for deleting the run manager.  If a valid physics
/// list name is defined, then that will be used for this run.
namespace EDepSim {
    G4RunManager* CreateRunManager(G4String physicsList);
}
#endif
