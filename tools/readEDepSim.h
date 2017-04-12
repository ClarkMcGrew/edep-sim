// TEST MACRO FOR USE WITH OLDER ROOT.  DOESN"T WORK WHEN CLING KNOWS ABOUT
// THE DATA CLASSES.
#ifndef readEDepSim_h
#define readEDepSim_h

#include <TTree.h>
#include "TG4Event.h"

/// Return a pointer to the EDepSimEvents Tree found in the geometry file.
TTree*    EDepSimTree();

/// Return a pointer to the i'th entry in the tree.  This is an event.
TG4Event* EDepSimEvent(int i);

/// Return a pointer to the last event read.  
TG4Event* EDepSimEvent();

/// Dump the current event in memory.
void EDepSimDumpEvent();
#endif
