# edep-sim : An Energy Deposition Simulation

The energy deposition simulation is a wrapper around the GEANT4
particle propagation simulation and is intended as a tool to simulate
all of the particle propagation and geometry related issues.  GEANT4
is an excellent toolkit, but it requires you to provide the main
programs, as well as the input, and output tools.  This has proven to
be a very good design choice; unfortunately, it means that there is no
standard wrapper code.  I've found myself using (and writing) almost
identical input/output wrappers several times, so edep-sim is intended
to provide a general starting point and allow users to concentrate on
the physics. As a standalone program, edep-sim provides a simple ROOT
based file format which will record the output of GEANT4.  It can also
be linked as a library into another application, and makes the same
information as a class.

This documentation assumes that you have a passing familiarity with how to
write GEANT4 macro control files.

Where it's available (and in particular for argon), the simulation
implements a fairly detailed model of the energy deposited as
ionization and non-ionizing (e.g. scintillation) energy loss.  The
liquid argon model uses using the NEST model (M Szydagis et al 2013
JINST 8 C10003 and references therein).  All other models use the EM
saturation model provided by geant 4.

Input file translators are provided for several standard input file
formats.  In particular, this support input from both NEUT and GENIE.  The
GENIE input is read using the RooTracker format.

Be aware that the sequence of random numbers depends on the floating
point accuracy of the machine, so given runs are machine dependent and
may not be reproducible.

## Working with the source distribution

It's quite possible that you are working in an environment where `edep-sim`
has already been installed (check with your local expert).  However, if you
are installing it from source, you need to run the local setup and then
build it.  Assuming you've already cloned it from someplace and have the
source directories, then `source` the setup script in the top level
directory to define some simple aliases and define necessary environment
variables.  The only explicitly external requirements are that GEANT and
ROOT must be available (and found by cmake).  The setup script will make
sure that they can be located (using `thisroot.sh` and `geant4.sh`).

```bash
. setup.sh
```

The simulation is built using cmake.  CMake can be run by hand, but there
is a script in the build directory that can be run using the `edep-build`
alias.  The build has been tested with GEANT4 10.2 and ROOT 6.08, but it
will probably build with any recent version of GEANT4 and ROOT.

If you have doxygen installed in a place where `cmake` can find it, then
you can compile using

```bash
edep-build doc
```

which will create the html sub-directory containing Doxygen class
documentation.  The html is in the build directory.

### Required configuration for GEANT4 and ROOT

The edep-sim code will build with very standard installations of
GEANT4 and ROOT, but a few non-default options are needed:

 * Requires GEANT4 GDML Persistence: "-DGEANT4_USE_GDML=ON"

 * Recommended GEANT4 Datafiles: "-DGEANT4_INSTALL_DATA=ON"

These options are already turned on in most GEANT4 and ROOT installations.

If you are compiling ROOT and GEANT4 by hand, following will generally
work (changed for your versions):

```bash
tar xvzf root_v6.22.00.source.tar.gz
mkdir root-6.22.00-install root-6.22.00-build
cd root-6.22.00-build
cmake -DCMAKE_INSTALL_PREFIX=../root-6.22.00-install ../root-6.22.00
make
make install
```

```bash
tar xzvf geant4.10.06.p02.tar.gz
mkdir geant4.10.06.p02-install geant4.10.06.p02-build
cd geant4.10.06.p02-build
cmake -DCMAKE_INSTALL_PREFIX=../geant4.10.06.p02-install \
      -DGEANT4_INSTALL_DATA=ON \
      -DGEANT4_USE_GDML=ON ../geant4.10.06.p02
make
make install
```

### Expert Compilation

This is actually just a generic cmake build system, so everything can be
done by hand.  It's a usual cmake build, but you need to make sure root and
geant are "in the path".  Assuming that you have the source in
"the-edep-sim-directory", will build in "the-build-directory", and want to
install in "the-install-directory", the commands are:

```bash
cd the-build-directory
cmake -DCMAKE_INSTALL_PREFIX=the-install-directory the-edep-sim-directory
make
make doc
make install
```

You can make sure that ROOT and GEANT are correctly setup using

``` bash
source thisroot.sh
source geant4.sh
```

## Running the Detector Simulation

The detector simulation is run using the `edep-sim` program which
is a standard GEANT main program with minimal customization.  This
can be run in any directory.  You will need to provide some sort of event
kinematics generator (NEUT, NUANCE, NEUGEN, GPS, &c).  The preferred input
format is a rooTracker tree, but you can also use the text based tracker
input format (NUANCE may be the only program in the world still using this
format.  It was developed as a standard kinematics format for the IMB3
experiment in about 1990)

The program is generally run from the command line:
```bash
edep-sim [options] macro-file.mac [macro-file.mac] ...
```
with one or more macro files.  The output file will contain a root tree of
event information and a TGeoManager object summarizing the simulated
geometry.  There are example input files in the inputs sub-directory.

Some important options:

* -e <n>  : Add a `/run/beamOn <n>` command after the last macro is
		processed so that `n` events are generated.
* -g <gdml-file> : Load a gdml file.  This is equivalent to the
		`/edep/gdml/read` macro command.
* -o <output-file> : Write the output to the root file.  This is equivalent
		to the `/edep/db/open` macro command.
* -u      : Add a `/edep/update` command before the first macro is
		processed.
* -p <physics-list> : Set the physics list (see G4 documentation for the
		available lists.
* -C      : Toggle checking for overlaps in the geometry.

If `edep-sim` is run with the `-h` option, it will print a help message.
There are example macro files in the "inputs" subdirectory.

EDepSim uses some GEANT4 macro files to control how it is run.  These are
not usually visible to the user, but occasionally the executable can't find
them.  In that case, you can specify the location by setting the
EDEPSIM_ROOT environment variable to the root of the INSTALLATION area.
For example

```bash
export EDEPSIM_ROOT=$(dirname $(which edep-sim))/..
```

should find the right location for the necessary files.

An example of a command line is

```bash
edep-sim -o my-output.root -g geometry.gdml -u -e 100 muon-10000.mac
```

which will write 100 events to `my-output.root` using the geometry in
`my-geometry.gdml` according to the commands in muon-10000.mac.

The muon-10000.mac might contain:
```
####################################################################
#
# Generate a 10000 MeV muon beamed in the down stream direction.
#

# Define the particle and it's energy.
/gps/particle mu+
/gps/energy 10000 MeV

# Set the position of the particle gun.
/gps/position 0.0 0.0 -10.0 m

# This generates the direction of the muon.  This aims
# the particle downstream with the directions uniformly distributed in
# a 5 deg cone around the Z axis.
/gps/ang/type iso
/gps/ang/maxtheta 5 deg
/gps/ang/rot1 1 0 0
/gps/ang/rot2 0 -1 0
```

## Reading the output

The output file is a standard root tree.  A simple example of how to read
the tree can be found in the test sub-directory (see the testTree.sh)
script and the associated readTree.C file.

The output is generic root with two keys: `EDepSimGeometry` for a
TGeoManager object with the simulated geometry, and `EDepSimEvents` for a
tree with the event data.  It can be read with any of your favorite ROOT
tree access techniques (e.g. `MakeProject`).  There are some ROOT ".C"
macro files in the tools directory, written to help test and debug the MC,
in the "tools" directory.  They can be helpful to process the file (and do
simple event displays).

All of the values in the output file are given in CLHEP units.  Namely,
distances are in millimeters, energies are in MeV, and times are in
nanoseconds.  The output file uses the global coordinate system defined by
the input geometry.  Be aware that in the saved TGeoManager object, the
masses and densities are also in CLHEP units, so that 1 kilogram equals
6.24x10^24^ MeV ns^2^ mm^-2^, and densities are in units of 6.24x^24^ MeV
ns^2^ mm^-5^.

#### Using the edepsim_io library

In addition to accessing the tree using "normal" root methods, you can also
directly line the edepsim_io library.  The i/o library can be compiled
without depending on GEANT4, and can be used via CMAKE.  If the EDepSim
package is in your path, the

```
find_package(EDepSim)
```

in a CMakeLists.text file will include the edepsim_io library as the
EDepSim::edepsim_io.  For example

```
# Configure the dependencies
find_package(EDepSim)

add_executable(my-code myCode.cc)
target_link_libraries(my-code
  PUBLIC EDepSim::edepsim_io)
```

This will automatically include any ROOT dependencies needed by edepsim_io,
and GEANT4 if necessary.

### Output Tree Format

The `edep-sim` executable produces a root `TTree` containing a single
branch.  You can access this from either C++, a root macro or python.  For
C++ based access, you must include the TG4Event.h header, and then attach
to the tree.

```C++
#include "TG4Event.h"

/// Get the event tree.
TTree* events = (TTree*) gFile->Get("EDepSimEvents");

/// Set the branch address.
TG4Event* event=NULL;
events->SetBranchAddress("Event",&event);

/// Get event zero.
events->GetEntry(0);
```

Or in python:
```python
# Get the event tree.
events = tFile.Get("EDepSimEvents")

# Set the branch address.
event = ROOT.TG4Event()
events.SetBranchAddress("Event",ROOT.AddressOf(event))

# Get event zero.
events.GetEntry(0)

print "event number", event.EventId
print "number of trajectories", event.Trajectories.size()
```

The detailed documentation is available via doxygen, but is summarized
here.

#### The TG4Event Class

The main event class (TG4Event) is a data-only class defined in the
`TG4Event.h` include file.  It has the following fields which should be
access using the matching accessor:

   * EventId: The event number

   * RunId: The run number

   * Primaries: The GEANT4 primary particles (A vector of
     TG4PrimaryVertex)

   * Trajectories: The GEANT4 particle trajectories (A vector of
     TG4Trajectory)

   * SegmentDetectors: The energy deposition information (A map keyed by
     sensitive detector name, containing a vector of TG4HitSegments).

#### The Trajectory Class (and Friends)

The trajectory information for each particle simulated in GEANT4 is
contained in a TG4Trajectory object.  This is a data-only class defined in
TG4Trajectory.h.  It has the deprecated public fields, and accessor that
should be used instead of the field:

  * TrackId [GetTrackId()]: The track identifier for this particular
      trajectory.  This is a positive integer.

  * ParentId [GetParentId()]: The track identifier for the parent of this
     trajectory.  This is an integer.  If the track is from a primary
     particle, the parent identifier will be zero.

  * Name [GetName()]: The type of particle creating this track.

  * PDGCode [GetPDGCode()]: The PDG MC code for the type of particle
    creating this track.

  * InitialMomentum [GetInitialMomentum()]: The initial momentum of track
    (A TLorentzVector).

  * Points: This has type TrajectoryPoitns and is equivalent to vector of
    TG4TrajectoryPoints along the track.

The TG4TrajectoryPoint is a data-only class with the following information.

  * Position [GetPosition()]: The position of the trajectory point (A
    TLorentzVector)

  * Momentum [GetMomentum()]: The momentum (a TVector3) as the trajectory
    leaves the point.

  * Process [GetProcess()]: The process type which created this point.
    These are defined by GEANT4.

  * Subprocess [GetSubprocess()]: The subprocess type which create this
    point.  These are defined by GEANT4.

##### Controlling which trajectories are saved

There are several macro commands provided which will allow the saved
trajectories to be filtered before they are saved to an output file.
Check the edep-sim-command-list.txt file and search for the
"/edep/db/set/" directory for details.

#### The Energy Deposition Class (and Friends).

The energy deposition in each sensitive detector is recorded using the
TG4HitSegment class.  This class records the starting and stopping position
of each energy deposition step (the energy is *not* deposited at a
point).  It also records the particle depositing the energy, as well as the
primary particle, or ultimate parent, associated with the energy deposit.

The generated hit segment contains fields with the following
information.  The ```public``` fields are deprecated, and you should
use the accessors instead of the field:

  * Contrib: A vector of track identifiers which contributed to this hit
    segment.  There is almost always only one contributor, but for certain
    run settings there may be several particles associated with one segment
    (this is a very unusual situation).  This has type
    TG4HitSegment::Contributors and is equivalent to a vector of integers.

  * PrimaryId [GetPrimaryId()]: The track identifier for the primary
    particle creating this hit.

  * EnergyDeposit [GetEnergyDeposit()]: The total energy deposited over the
    length of this track.  The energy should be assumed to have been
    uniformly deposited along the segment (*not* at the beginning or the
    end).  This is the total dEdX of the track between the start and stop
    position of the segment.

  * SecondaryDeposit [GetSecondaryDeposit()]: The "secondary" energy
    deposited over the length of the segment.  This is generally used to
    help simulate the energy emitted as scintillation light vs the total
    dE/dX of the track.  In other words, when the secondary deposit
    simulation is turned on, This is the energy deposited as optical
    photons.  The remaining energy is usually deposited as ionization.  For
    example, in liquid argon, the mean number of quanta crated will be N~q~
    = (EnergyDeposit/W~Ar~), where W~Ar~ is the work function for argon
    (typically 19.5 eV). The number of optical photons is N~ph~ = N~q~ *
    SecondaryDeposit/EnergyDeposit, and the number of ionization electrons
    is N~e~ = N~q~ - N~ph~.  For liquid argon the Doke-Birks model as
    implemented by NEST used.

  * TrackLength [GetTrackLength()]: The total track length between the
    start and stop points (as estimated by GEANT4)

  * Start, Stop [GetStart(), GetStop()]: The starting and stopping points
    of the segment (TLorentzVector).


##### Controlling the step size.

The hit segment length is determined by a combination the GEANT4
physics step and a segment target length.  The GEANT4 physics step
length is determined by the interaction cross sections, the energy
loss, and the multiple scattering.  The physics step length control
parameters (but not explicitly the step length) can be controlled
using the macro commands in the ```/process/eLoss/``` and
```/process/msc/``` directories.  A geometry can set an explicit step
length limit for a logical volume (see below).  The steps taken by
GEANT4 are summarized by hit segments (see above), that will combine
steps until the next step would make the segment longer than the
target length.  The target length of the hit segment can be controlled
using the ```/edep/hitLength``` macro, and the maximum sagitta of the
segment can be controlled using the ```/edep/hitSagitta``` macro.
Notice that the segment will grow until it is approximately as long as
the target hit length value, but depending on the underlying
simulation, will probably be shorter, or longer than the target.  Typically,
the segment length should be a little smaller than the resolution of the
detector.  Hit segments will not cross geometry boundaries.

Note: All of the step length controls must occur during the
```PREINIT``` stage.  In practice, this means they need to occur
before the ```/edep/update``` command has been given.  You can
explicitly add the ```/edep/update``` command to the job macro, or it
will be added automatically using the -u command which will insert an
```/edep/update``` *before* running the macro on the command line.

It is important to notice that the hit segments can have a length that is
long compared to the resolution of the detector, but the "diameter" of the
segment controlled by the hitSagitta, energy loss and multiple scattering
parameters will be small so the segment can be represented by a straight
line between the starting and stopping points.  The hitSagitta and physics
stepping parameters should be set so that that the maximum displacement of
the track from a straight line is small compared to the detector
resolution.

### Simple Debugging Display

There is a simple Eve based event display that will read the ROOT output
file and.  It is not a complete event display, but can be useful to help
debug geometries, and input kinematics files.  It can also be helpful to
understand what events "look like".  It can be run using the `edep-disp`
command.  A help message is printed if it is run without any arguments.

## Specifying The Detector Geometry.

The detector geometry can be specified in more than one way.  The
first is to compile it into the executable by defining the appropriate
EDepSimBuilder classes.  There is an example of a simplified geometry
for the CAPTAIN LArTPC contained in the source.  The alternative is to
import a GDML definition using the ```/edep/gdml/read``` macro
command.  The gdml file can be specified from the command line using
the ```-g``` option

```bash
edep-sim -g geometry.gdml -o output.root macro-file.mac
```

### GDML Logical Volume Auxiliary Fields

Several auxiliary fields are parsed to help describe volumes in the
detector geometry and attach more meaning to the GDML description.
For example, here is a simple LAr detector description that is
attached as a sensitive detector, and has both an electric and a
magnetic field.

```
    <volume name="LArD_lv">
      <materialref ref="LAr"/>
      <solidref ref="LArD"/>
      <physvol>
        <volumeref ref="LArMod_lv"/>
        <positionref ref="LArD_el0_pos"/>
        <rotationref ref="identity"/>
      </physvol>
      <auxiliary auxtype="SensDet" auxvalue="LArD"/>
      <auxiliary auxtype="EField"
		  auxvalue="(500.0 V/cm, 1.0 V/m, 500.0 V/cm)"/>
      <auxiliary auxtype="BField
		  " auxvalue="(2.0 tesla, 3.0 T, 1.0 G)"/>
      <auxiliary auxtype="StepLimit" auxValue="1.0 cm"/>
    </volume>
```

#### Auxiliary field to set a sensitive volume

Sensitive logical volumes recording energy deposition are specified using
the "SensDet" type.  It takes a value which is the name of the sensitive
detector that will record the deposition.

```
<auxiliary auxtype="SensDet" auxvalue="MySensDetName"/>
```

In the example above, the energy is recorded in a sensitive detector named
`MySensDetName`.  After an event is simulated, the deposited energy is
recorded in an internal map keyed by the sensitive detector name, and with
a value that is a vector of TG4HitSegment objects.  See the example in the
`./test` subdirectory for how to read this information from the output
tree.

#### Auxiliary field to define the electric and magnetic field.

Uniform electric and magnetic fields can be set using the `EField` and
`BField` auxiliary types.  The value for each is the field vector in
the volume specified by the X, Y and Z components (global
coordinates).  It is assumed that the field applies to all of the
daughter volumes.

```
<auxiliary auxtype="EField" auxvalue="(500.0 V/cm, 1.0 V/cm, 500.0 V/cm)"/>
<auxiliary auxtype="BField" auxvalue="(2.0 T, 3.0 T, 1.0 G)"/>
```

The possible units for the electric field are `volt/cm`, `volt/m`,
`V/cm`, and `V/m`.  You should specify a unit.  If no units are
provided, it is assumed that the units are `volt/cm`.

The possible units for the magnetic field are `tesla`, `gauss`, `T`,
and `G`.  If no units are provided, it is assumed that the units are
`tesla`.

Arbitrary electric and magnetic fields can be set using the
`ArbEField` and `BField` auxiliary types. The value for each is the
name of the grid file which specifies the value of the field at a set
of X, Y, and Z grid points (global coordinates).

```
<auxiliary auxtype="ArbEField" auxvalue="efield_grid_file.txt"/>
<auxiliary auxtype="ArbBField" auxvalue="bfield_grid_file.txt"/>
```

The grid file is specified in the following format:

```
# First row is a header defining the origin offset and grid spacing
# in each position coordinate X, Y, Z then hX, hY, hZ.
-2600.00 -4200.00 12300.00 200.00 200.00 200.00

# Next, each row contains one grid point: x,y,z,fx,fy,fz,f
0.00 0.00 000.00 0.40 0.00 0.00 0.40
0.00 0.00 200.00 0.40 0.40 0.00 0.57
0.00 0.00 400.00 0.40 0.40 0.40 0.69
...and so on...
```

The position, XYZ, is specified in `mm`, the electric field is
specified in `V/cm`, and the magnetic field is specified in `T`. The
grid spacing for each axis can be different.

FX, FY, FZ are the magnitudes of the fields in the X,Y,Z directions
and F is the total magnitude of the field vector. The Z coordinate
iterates first, then Y, and finally X when incrementing the lines.

To save space, the position of each point is not stored. Instead the
position is calculated using the offset and the index of the array: `x
= hx * i + offset_x`, where hx is the grid spacing in x, i is the ith
element of the array, and offset_x is the offset specified in the
first line for the x coordinate.

Comment lines starting with the `#` character are supported in the
grid file. They will be ignored when parsing the file.

#### Auxiliary field to set the drawing color for the volume

The display properties for the logical volume can be set using the `Color`
and `Opacity` auxiliary types.

The color of a volume can be set in two forms.  The first form allows
a few primary colors to be set by names.  These colors are defined by
the G4Colour class and are one of the list "white", "gray", "grey",
"black", "brown", "red", "green", "blue", "cyan", "magenta", and
"yellow".  A color can be set with one of these as the value.

```
<auxiliary auxtype="Color" auxvalue="red"/>
```

The color can also be set as an RGB triplet, or an RGBA quartet.

```
<auxiliary auxtype="Color" auxvalue="(1.0,0.0,0.0)"/>
<auxiliary auxtype="Color" auxvalue="(1.0,0.0,0.0,0.5)"/>
```

The fourth component of the color is the "alpha" which can run between
zero and one.  A value of zero is transparent, and a value of one is
opaque.  The alpha value can also be set using the opacity auxiliary
type.

```
<auxiliary auxtype="Opacity" auxvalue="0.5"/>
```

#### Auxiliary field to set the volume step limit.

The step limit can be set using

<auxiliary auxtype="StepLimit" auxValue="1.0 cm"/>

The value takes a number and a unit.

## Describing Detector Materials

Detector materials are described using the standard GEANT4 methods,
but with a few minor extentions to simplify usage.  For geometries
that are coded using the native GEANT4 routines (i.e. implementing a
specialized UserDetectorGeometry class and built by deriving builders
from EDepSimBuilder) should mostly depend the standard GEANT4
interface.

### Specifying Saturation for a Material (The Birks Constant)

Internally, GEANT4 manages the Birks constant for a material using
stippets that look like

```C++
G4Material* material = G4Material::FindMaterial("Scintillator");

// Set the Birks' constant for a material
double birks_constant = 0.12;
material->GetIonization()->SetBirksConstant(birks_constant);

// Get the Birks' constant for a material
birks_constant = material->GetIonization()->GetBirksConstant();
```

Unfortunately, this interface is not exposed when defining materials
using GDML.

#### Specifying the Birks' constant in a macro

As a work-around, there is an ```edep-sim``` macro that will
override the Birks' constant for an existing material.  The macro must
be used immediately before using the ```/edep/update``` macro.  The
Birks' constant is set using

```
/edep/material/birksConstant [material] [value] [unit]
```

where ```material``` is the name of an existing material, ```value```
is a floating point number (e.g. 0.126), and ```unit``` is a string
describing the units (e.g. ```mm/MeV```).  The unit must be a
```[Length]/[Energy]```, and must contain a '```/```', however there is
only minimal error checking to make sure the result is reasonable.
For standard polystyrene (which must have already been defined), the
Birks' constant would be set using

```
/edep/material/birksConstant Polystyrene 0.126 mm/MeV
/edep/update
```

An error is thrown if the material has not been defined, or if the
units are malformed (e.g. the unit is missing a '/')

#### Specifying the Birks' constant using a custom material property

While GDML does not provide an element that will directly set the
Birks' constant value for a material, it does define and interface
allowing material properties to be set.  For instance,

```
<define>
   <matrix name="BIRKSVALUE" coldim="1" values="0.126"/>
</define>
<materials>
  <material formula="Scintillator">
     <property name="BIRKSCONSTANT" ref="BIRKSVALUE"/>
  </material>
</materials>
```

will add a constant material property to the material properties table
for the "Scintillator" material.  By default, GEANT4 will ignore these
extra properties, but ```edep-sim``` has been modified to look for
specific properties and to use them to set values in the ```G4Material```
class.  Properties that will be interpreted are

* ```BIRKSCONSTANT``` -- Set the Birks' constant value.  The value
    must be in units of ```mm/MeV```.

## Running as a library

The simulation is developed as a library separated from a main program
wrapping the library.  The entire main program is only a few hundred
lines long, of which about twenty lines are needed to actually setup
and run the simulation.  It is relatively straight forward to take
this and include edep-sim into a separate package.  The example main
program (i.e. app/edepSim.cc) is commented and the library is started
beginning at about line 230.

### Using as a CMAKE package

The simulation is available for CMake projects using the find_package
macro.  It can be included using
```
find_package(EDepSim)
```
and then added to an executable using the
```
target_add_library(your-target PUBLIC EDepSim::edepsim)
```
which will add all of the required dependencies.

If you only want to use the i/o library, and don't need access to geant4,
you can use
```
target_add_library(your-target PUBLIC EDepSim::edepsim_io)
```
which only depends on ROOT.

## Specifying the Primary Particle Kinematics

The primary particle kinematics are controlled using the `/generator` macro
commands.

### Using GENIE inputs

```
## Don't repeat the random number sequence (not limited to GENIE input)
/edep/random/timeRandomSeed

## Replace this with the name of a GENIE rooTracker file
/generator/kinematics/rooTracker/input <rooTracker-file>

## Use the rooTracker input format.  This is directly supported by GENIE.
/generator/kinematics/set rooTracker

## Place one interaction per event
/generator/count/fixed/number 1
/generator/count/set fixed

## Distribute the events based on the density of the material.  When done
##   this way, the composition of the detector is ignored, so it's not the
##   best way for physics, but it's good for an example since you don't
##   need to syncronize the GENIE and EDEPSIM geometries.
/generator/position/set density

## Make sure EDEPSIM updates the kinematics generator.
/generator/add
```

### Controlling the primary vertex timing

The primary vertex timing is often taken directly from the kinematics
generator, or set to be at time equal to zero, but it can be
overridden to simulation the event spill.  To control the primary
vertex timing distribution use the following macro commands
(substitute the values you want.

Set the vertices to occur at a specific time
```
/generator/time/fixed/time 100 ns
/generator/time/set fixed
```

Let the vertex use the time provided by the kinematics generator.
This is the default behavior, and usually results in a vertex time of
zero

```
/generator/time/set free
```

Distribute the event according to a beam spill timing distribution.
The following macros will approximate the FNAL beam structure.

```
/generator/time/spill/start 0 ns
/generator/time/spill/bunchCount 1000
/generator/time/spill/bunchSep 10 ns
/generator/time/spill/bunchLength 5 ns
/generator/time/set spill
```

There are further commands to provide limited control over the power
of each bunch.

### Using GPS (The General Particle Source)

This describes how to use the GPS particle source provided by GEANT.  It is
part of GEANT, so deviates from the normal way that EDEPSIM accepts
kinemtics.

This particle source is incredibly useful, but extremely is idiosyncratic.
It's also has relatively little documentation, and the documentation that
exists is often misleading.  This page attempts to cover the basics of
using GPS to write GEANT macros that produce samples of events to be used
for testing ND280 software (meaning I got annoyed enough at the GPS
documentation make notes).

The general particle source as developed for the ESA, so it tends to use
astronomical conventions.  This is particularly relevant with describing
angles and directions.  Astronomers (and apparently space scientists) tend
to speak about where the particle is coming from, not where it is going.
This makes for an odd match when dealing with a particle simulation which
is concerned with tracking the particles, and not determining the position
of the source.  Once you get beyond the idiosyncratic definitions (which
are at odds with the rest of GEANT), the GPS can be used to create almost
any sort of particle distribution for testing programs.

#### Controlling the Simulated Particle

The particle being simulated is set using the `/gps/particle` command.  The
particles are set by name, and can be any of the ones simulated by GEANT.
The most common particles are `mu-`, `mu+`, `pi+`, `pi-, `e-`,
`e+`, `gamma`, and `proton`.  For example

```
/gps/particle mu-
/gps/particle pi+
/gps/particle gamma
/gps/particle proton
```

#### Controlling the Position Distribution

The vertex position is controlled using the commands in the /gps/pos/
subdirectory.  The most useful distributions are the Point, Plane, and
Volume.  To generate a point source the following commands are used

##### Making a point source

```
/gps/position 0 0 -50 cm
/gps/pos/type Point
```

where the first line specifies that this is a point and the second line
specifies the position.  Notice that all three coordinates must have the
same units.  For a point source, it's not necessary to specify the type.
GEANT is very inconsistent about the spelling of `centre` vs `center` with
19 classes using `centre` and 33 using `center`.  You just have to check
which version is being used by which class.  For consistency, avoid the
problem by prefering `/gps/position` and avoiding `/gps/pos/centre`.

##### Making a rectangular or circular source

Since a point source can bias tests, vertices should usually be spread over
a plane or a volume.  The vertices can be distributed over a flat surface
by setting

```
/gps/position 0 0 -50 cm
/gps/pos/type Plane
/gps/pos/shape Rectangle
/gps/pos/halfx 1 cm
/gps/pos/halfy 1 cm
/gps/pos/rot1 0 0 -1
/gps/pos/rot2 0 1 0
```

##### Making a volume source

To distribute the vertices over a volume the type is changed to Volume, and
a volumetric shape must be chosen.  The following commands will generate a
box

```
/gps/position 0 0 -50 cm
/gps/pos/type Volume
/gps/pos/shape Para
/gps/pos/halfx 1 cm
/gps/pos/halfy 1 cm
/gps/pos/halfz 1 cm
```

that is 100 by 100 by 10 cm.  It's important that the /gps/position command
comes first.  The other shapes that might be useful are Sphere and Cylinder
which are specified as

```
/gps/position 0 0 -50 cm
/gps/pos/type Volume
/gps/pos/shape Sphere
/gps/pos/radius 1 cm
```

and

```
/gps/position 0 0 -50 cm
/gps/pos/type Volume
/gps/pos/shape Cylinder
/gps/pos/radius 1 cm
/gps/pos/halfz 1 cm
```

#### Controlling the Direction Distribution

The direction distribution is set using the commands in the /gps/ang/
subdirectory.  When specifying the direction, be aware that the code was
written under contract to the ESA, and no effort was made to be consistent
with the rest of the GEANT code base.  As a result, the directions use a
different angle convention when compared to the rest of GEANT.  In fact,
while the documentation says that the /gps/ang/ commands control the
angular distribution of the source, they actually control the direction
from a point along an initial particle direction ray back to where the
source is found [note that the mathematics are very poorly defined].  To be
honest, it's an odd and poorly thought out mix of definitions used by
physicists and astronomers.  Specifically, to get a particle traveling in
the positive Z direction, you specify a theta of 180 degree.

The coordinate system used to generate the direction can be changed by
using the /gps/ang/rot1 and /gps/ang/rot2 commands.  The X axis of the
direction coordinate system is defined by the /gps/ang/rot1 command, and
the XY plane is determined by the /gps/ang/rot2 command.  Neither of the
axes need to be unit vectors, and the rot2 vector doesn't need to be
perpendicular to rot1 (the normal will be determined by rot1 cross rot2).
The directions will then be distributed around the negative Z axis in the
rotated coordinate system.  This means that

```
/gps/ang/rot1 0 0 1
/gps/ang/rot2 0 1 0
```

will rotate the direction coordinate system so that the local Z axis points
along to global negative X axis.  This will cause the generated particles
to travel along the positive X axis (i.e. along the local negative Z axis).
Note: The default for /gps/ang/rot2 is [0 1 0], so it doesn't need to be
specified.

##### Setting a fixed particle direction

A fixed particle direction can be specified using

```
/gps/direction 0 0 1
```

where this specifies the direction that the particle is traveling.  In this
case, the particle is traveling along the positive Z axis.  Notice unlike
the rest of the gps direction commands, this uses the same convention as
the rest of GEANT.

##### Making directions with a Gaussian spread

The problem with specifying a fixed direction is that it may bias a test.
A Gaussian direction spread can be created using a one dimensional beam
traveling along the local negative Z axis (yes, it is a bizarre choice).
For instance, to create a beam traveling along the Z axis with a 10 degree
divergence (sigma) the following commands would be used.

```
/gps/ang/type beam1d
/gps/ang/sigma_r 10 deg
/gps/ang/rot1 0 0 1
/gps/ang/rot2 0 1 0
```

The first line specifies that a 1D beam should be generated where this
means that the generated directions have a spread around the negative Z
axis.  The second line specifies a 10 degree spread around the negative Z
axis.  The third line fixes the screwball convention that the beam travels
along the negative Z axis by reversing the coordinate system (this aligns
the direction coordinate system so that it's negative Z axis points along
the global Z axis).  You should be aware that (for reasons completely
beyond me, but I've verified it in the source code) the /gps/ang/maxtheta
and /gps/ang/mintheta commands have no effect on the beam distributions.

##### Generating directions in a cone

To spread the directions isotropically over all direction, the following
command is used

```
/gps/ang/type iso
```

The directions can then be limited to a cone using the max and min
commands.  By default, the maximum angle is 180 degree and the minimum is 0
degree.  Bear in mind that 180 degrees points along the positive Z axis and
0 degrees points along the negative Z axis (these guys failed high school
geometry).  To create a 10 degree cone of directions along the Z axis use

```
/gps/ang/type iso
/gps/ang/mintheta 170 deg
```

or alternatively

```
/gps/ang/type iso
/gps/ang/maxtheta 10 deg
/gps/ang/rot1 0 0 1
/gps/ang/rot2 0 1 0
```

where the final line deals with the wacky angle convention.

#### Controlling the Energy Distribution

The energy distribution is set using the commands in the /gps/ene/
subdirectory.  There are many options available, but for testing programs
the most useful ones are to create a mono-energetic, Gaussian, or uniform
energy source.  By default the energy is specified as the kinetic energy of
the created particle.

##### Making a monoenergetic source

To create a monoenergetic source, the energy is specified
like this:

```
/gps/ene/mono 100 MeV
```

where you must specify both the energy value and the unit.  This create a
monoenergetic source with 100 MeV of kinetic energy.

##### Making a uniform energy distribution.

A uniform energy distribution can be created using:

```
/gps/ene/type Lin
/gps/ene/gradient 0
/gps/ene/intercept 1
/gps/ene/min 100 MeV
/gps/ene/max 500 MeV
```

where this creates a source with a uniform kinetic energy distribution
between 100 MeV and 500 MeV of kinetic energy.  The gradient is specified
in MeV^{-1}.  The intercept is a pure number, and the final distribution
does not need to be normalized.

##### Making a Gaussian energy distribution

A Gaussian energy distribution can be created using

```
/gps/ene/type Gauss
/gps/ene/mono 400 MeV
/gps/ene/sigmae 30 MeV
```

where this create a source with a Gaussian spread of 30 MeV around 400 MeV.

#### Creating Multiple Particle Events

The GPS is very inflexible when it comes to handling multiple
particles in a single event, so it's not possible to handle the
general case with multiple particles coming from the same vertex.
This is overcome in detSim by implementing the `/generator/combine`
command which is discussed below.

For the simple case of generating multiple particles drawn from the
same angle and energy distributions, but with the same position, use
`/gps/number` command.  For instance

```
/gps/number 2
```

would create two particles at the vertex.  Note that both particles have
the same energy and position distributions.

If you need control over the individual particles, then you have to
use the /gps/source commands in combination with the /generator/combine
command.  For instance, to create an event with a mu- and a proton from the
same vertex position (notice there will be two "primary vertexes" with the
same position coordinates), you could create a macro:

```
# Clear the source and tell GPS to make multiple vertices
/gps/source/clear
/gps/source/multiplevertex true

# Create the first particle.  This can be done using any of the GPS macro
# commands.
/gps/source/add 1
/gps/particle mu-
/gps/energy 500 MeV
/gps/direction 0 1 1
/gps/position 0.0 0.0 -50 cm
/gps/pos/type Volume
/gps/pos/shape Para
/gps/pos/halfx 1 cm
/gps/pos/halfy 1 cm
/gps/pos/halfz 1 cm

# Create the second particle.  This can be done using any of the GPS macro
# commands.  The position will be overridden below.
/gps/source/add 2
/gps/particle proton
/gps/energy 300 MeV
/gps/direction 0 0 1
/gps/position 0 0 0

# Add the GPS generator.  It will create two primary vertices
# (G4PrimaryVertex objects).
/generator/add

# Copy the vertex position from the first G4PrimaryVertex object to the
# second.  Notice that the vertices in GPS are numbered from one, but that
# the G4PrimaryVertex objects are numbered from zero.

/generator/combine 0 1
```

The `/generator/combine` command copies the vertex position from the first
vertex to the second, and takes three arguments.  The first argument is the
vertex number to copy the position, the second argument is the vertex
number to copy the position into, and the optional third argument is a
boolean flag specifying if the position of the second vertex should be
relative to the first.  If the argument is true, then the position is of
the second particle is adjusted to be relative to the position of the first
(default: false).  This can be used to simulate displaced vertices.  As an
example, if the optional third argument is true, the first vertex position
is [1,1,1], and the second vertex position is [0,1,0], then the simulated
vertex positions will be [1,1,1] and [1,2,1].
