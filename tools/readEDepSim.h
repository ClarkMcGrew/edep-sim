#ifndef readEDepSim_h
#define readEDepSim_h

#include "EDepSimEvents/EDepSimEventsProjectHeaders.h"

#include <TTree.h>

/// Return a pointer to the EDepSimEvents Tree found in the geometry file.
TTree*    EDepSimTree();

/// Return a pointer to the i'th entry in the tree.  This is an event.
TG4Event* EDepSimEvent(int i);

/// Return a pointer to the last event read.  
TG4Event* EDepSimEvent();

/// Dump the current event in memory.
void EDepSimDumpEvent();
#endif
