#ifndef DSimCreateRunManager_hh_seen
/////////////////////////////////////////////////////////////////////
// $Id: DSimCreateRunManager.hh,v 1.2 2009/04/09 00:39:00 gum Exp $

#include <G4RunManager.hh>
#include <G4VUserPhysicsList.hh>

/// Create the standard run manager for the detSim detector simulation.  The
/// caller is responsible for deleting the run manager.  If a valid physics
/// list name is defined, then that will be used for this run.
G4RunManager* DSimCreateRunManager(G4String physicsList);

#endif
