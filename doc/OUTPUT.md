## Accessing the output with EDepSim::PersistencyManager

After an GEANT4 event is processed, the event is summarized using the EDepSim::PersistencyManager class which is a base class that users can specialize for their specific purposes.  The PersistencyManager class will marshal summaries of the event into local classes and provides the main access to the event.  By default, the output is written to a ROOT file with classes described by the `edepsim_io` library (see below).

### Reading the output produced by EDepSim::RootPersistencyManager.

The RootPersistencyManager is a light weight wrapper around the
PersistencyManager that saves the classes to a root tree. A simple example
of how to read the tree can be found in the test sub-directory (see the
testTree.sh) script and the associated readTree.C file.

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

   * SubrunId: The subrun number

   * Primaries: The GEANT4 primary particles (A vector of
     TG4PrimaryVertex)

   * Trajectories: The GEANT4 particle trajectories (A vector of
     TG4Trajectory)

   * SegmentDetectors: The energy deposition information (A map keyed by
     sensitive detector name, containing a vector of TG4HitSegments).

   * PhotonDetectors: The photons collected by HitSurface sensitive
     detectors.  This is a map keyed by the sensitive detector name and
     containing a vector of TG4PhotonHit objects.

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

#### The Energy Deposition Hit Class (and Friends).

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
    run settings it is possible for several particles associated with one
    segment (this is a very unusual situation).  This has type
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

#### The Optical Photon Hit Class

Optical photons are recorded in the TG4PhotonHit class. This class record
information about photons that were tracked and which were sensed by a
G4OpBoundaryProcess attached to a G4LogicalSurface.  Only photons with a
`Detection` status will generate a TG4PhotonHit.  The information contained
includes the final position of the photon (this is how the optical sensor
is specified), the energy (or wavelength) of the photon, and when
available, the track Id of the particle creating the photon, the starting
position of the photon, and the process which created the photon.  The
fields are accessed using the methods:

  * TLorentzVector GetPosition() const: The final position of the photon.

  * TLorentzVector GetStart() const: The starting position of the
    photon. This may not be filled if the starting position is not
    available from GEANT.

  * double GetEnergyDeposit() const: The energy of the photon in HEPUnits
    (ev).

  * double GetWavelength() const: The wavelength in HEPUnits (mm) derived
    from the energy of the photon.

  * int GetPrimaryId() const: The track id of the parent particle. This may
    not be filled if the information is not available from GEANT.
