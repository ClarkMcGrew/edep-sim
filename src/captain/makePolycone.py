#!/usr/bin/env python

import os
import sys
import getopt

def usage():
    print """
    makePolycone <inner-surface.txt> <outer-surface.txt>

    Outputs text to the standard output that is suitable to be included
    into a C++ source file defining the inner and outer surfaces of a
    G4Polycone.

    --vector-name <name>
           The name of a vector to be used in the include file.

    --step
           The step size along the Z axis to use while building the
           polycone.  This can be small (but it will make this run slowly),
           since only the steps where the precision is bad will be saved.

    --precision
           Set the maximum distance the wall of the cryostat can be from the
           interpolated line.
        """
    
try:
    options, args = getopt.gnu_getopt(sys.argv[1:], 
                                      "h",
                                      ["vector-name=",
                                       "precision=",
                                       "step=",
                                       "help"])
except:
    usage()
    print "ERROR: Invalid option."
    sys.exit(1)

if len(args)<2:
    usage()
    sys.exit(1)

# Define the default values for the options.
vectorName = "fCaptainShape"
boundaryStep = 3
precision = 0.3

# Parse the command line options.
for o, a in options:
    if o == "--vector-name":
        vectorName=a
    elif o == "--precision":
        precision=float(a)
    elif o == "--step":
        boundaryStep=float(a)
    else:
        usage()
        sys.exit(1)

# Read the points from an input file.  The file should be defined as
# X, Y, index Where +x is the top of the cryostat, -x is the bottom,
# and the Y direction is for the sides.
def ReadPoints(fileName):
    points = []
    for line in open(fileName):
        x = [float(val) for val in line.split()]
        points.append([x[0],x[1]])
    return points

# Find the radius of the cryostat at a particular vertical position.
# This is called "z" since it's going to be used to buill a G4Polycone
# along the Z axis.  In the Z axis here is equivalent to the X axis in
# the input file.
def FindRadius(z,points):
    radii=[]
    there=points[-1]
    for here in points:
        if ((there[0] <= z and z < here[0])
            or (here[0] <= z and z < there[0])):
            r = (here[1]-there[1])/(here[0]-there[0])*(z-there[0]) + there[1]
            radii.append(r)
        there = here
    if len(radii) == 0: return 0.0
    average = reduce(lambda x, y: x+y, radii)/len(radii)
    r = 0.0
    for p in radii:
        r = r + abs(p-average)
    return r/len(radii)

# Find the total range along the vertical axis of the cryostat that
# needs to be used to define the polycone.
def FindRange(inner,outer,step):
    minZ = min(innerPoints[0][0],outerPoints[0][0])
    maxZ = max(innerPoints[0][0],outerPoints[0][0])
    for p in inner:
        minZ = min(p[0], minZ)
        maxZ = max(p[0], maxZ)
    for p in outer:
        minZ = min(p[0], minZ)
        maxZ = max(p[0], maxZ)
    return xrange(int(minZ/step)-1, int(maxZ/step)+2)

# Check to see if a new polycone segment needs to be started.
def SegmentIsOK(p1,p2,middle,thres):
    for p in middle:
        r1 = (p1[1]-p2[1])/(p1[0]-p2[0])*(p[0]-p2[0]) + p2[1]
        r2 = (p1[2]-p2[2])/(p1[0]-p2[0])*(p[0]-p2[0]) + p2[2]
        if abs(r1-p[1]) > thres: return False
        if abs(r2-p[2]) > thres: return False
    return True

innerPoints = ReadPoints(args[0])
outerPoints = ReadPoints(args[1])

boundary = []
for iZ in FindRange(innerPoints,outerPoints,boundaryStep):
    z = boundaryStep*iZ
    innerRadius = FindRadius(z,innerPoints)
    outerRadius = FindRadius(z,outerPoints)
    boundary.append([z,innerRadius,outerRadius])

savedPoints = [boundary[0]]
runningPoints = []
for p in boundary[1:]:
    if len(runningPoints) < 1:
        runningPoints.append(p)
        continue
    if SegmentIsOK(savedPoints[-1],p,runningPoints,precision):
        runningPoints.append(p)
        continue
    savedPoints.append(runningPoints[-1])
    runningPoints = []
if savedPoints[-1] != boundary[-1]: savedPoints.append(boundary[-1])
                                                       
print """
////////////////////////////////////////////////////////////////////////
// This is an auto-generated built using makePolycone.py.  It defines
// points to be used to build a G4Polycone. It is built from the input
// files %s and %s.
// Prior to including this file, the vector %s must
// be define in a way equivalent to
// 
// class Point {
//  public:
//      Point(double z, double i, double o): fZ(z), fInner(i), fOuter(o) {}
//      double fZ;
//      double fInner;
//      double fOuter;
//  };
//
// std::vector<Point> %s;
//
////////////////////////////////////////////////////////////////////////
""" % (args[0], args[1], vectorName, vectorName)
    
for p in savedPoints:
    print "%s.push_back(Point(%f,%f,%f));" % (vectorName,p[0],p[1],p[2])

sys.exit(0)
