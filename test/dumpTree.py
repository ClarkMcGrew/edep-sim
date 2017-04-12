#!/usr/bin/env python

import sys
import os.path
import ROOT

# Get the input file.
if len(sys.argv) < 2: sys.exit()
fileName = sys.argv[1]
tFile = ROOT.TFile(fileName,"OLD")

# Make sure the classes are available.  If edepsim is in the path,
# this just makes (or loads) an empty library.
if os.path.exists("EDepSimEvents/EDepSimEvents.so"):
    ROOT.gSystem.Load("EDepSimEvents/EDepSimEvents.so")
else:
    tFile.MakeProject("EDepSimEvents","*","RECREATE++")

# Get the geometry.
tFile.Get("EDepSimGeometry")

# Get the event tree.
events = tFile.Get("EDepSimEvents")

# Example of looping over the events.
for entry in events:
    print "Have event", entry.Event.EventId
    print "Have primaries", entry.Event.Primaries.size()
    print "Have primaries", entry.Event.Primaries[0].GeneratorName
    
# Set the branch address.
event = ROOT.TG4Event()
events.SetBranchAddress("Event",ROOT.AddressOf(event))

# Get event zero.
events.GetEntry(0)

print "event number", event.EventId
print "number of trajectories", event.Trajectories.size()
for i in  event.SegmentDetectors:
    print "Detector name:", i.first, "Hits:", i.second.size()


