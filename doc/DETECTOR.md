# Describing the Detector and Materials

The detector description can be built using C++ classes derived from the
underlying GEANT4 methodology, or using extended GDML (the prefered
method). The recommendation is to use
[GeGeDe](https://github.com/brettviren/gegede/) to build a GDML file
describing the geometry and materials.

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
      <auxiliary auxtype="SegmentDetector" auxvalue="LArD"/>
      <auxiliary auxtype="EField"
		  auxvalue="(500.0 V/cm, 1.0 V/m, 500.0 V/cm)"/>
      <auxiliary auxtype="BField
		  " auxvalue="(2.0 tesla, 3.0 T, 1.0 G)"/>
      <auxiliary auxtype="StepLimit" auxValue="1.0 cm"/>
    </volume>
```

#### Auxiliary fields to set a sensitive volume

Sensitive logical volumes recording energy deposition are specified using
the "SegmentDetector", or "SurfaceDetector" types.  These take a value
which is the name of the sensitive detector that will record the
deposition.

```
<auxiliary auxtype="SegmentDetector" auxvalue="MySegmentDetectorName"/>
```

In the example above, the energy is recorded in a sensitive detector named
`MySegmentDetectorName`.  After an event is simulated, the deposited energy
is recorded in a map keyed by the sensitive detector name, and
with a value that is a vector of TG4HitSegment objects.  See the example in
the `./test` subdirectory for how to read this information from the output
tree.

```
<auxiliary auxtype="SurfaceDetector" auxvalue="MySurfaceDetectorName"/>
```

In the example above, the energy at surfaces (primarily optical photons) is
recorded in `MySurfaceDetectorName`.  After an event is simulated the hits
from opticalphotons are recorded in a map keyed by the sensitive detector
name and a value that is a vector of TG4PhotonHit objects.

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
snippets that look like

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
