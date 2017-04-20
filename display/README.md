# Debugging Display for edep-sim

This is a simple debugging display to help visually validate the output of
edep-sim.  It's not a full (real) event display!  It's included because it
can be very helpful for getting a handle on what is going on in an event,
but is not a substitute for a real display.

This expects a root file produced by the EDepSim::RootPersistencyManager.
It must contain a TGeoManager object named EDepSimGeometry, and a tree
named EDepSimEvents.

## Running

This can be run using "edep-disp <input-file>".  Or as "edep-disp -h" to
get a list of the command line parameters.

