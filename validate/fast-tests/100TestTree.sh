#!/bin/sh
#
# Write a ROOT file in the default geometry.  This output file should
# be tested to make sure that all of the fields are readable.

OUTPUT=100TestTree.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > 100TestTree.mac <<EOF
#######################################
# Set the hit segment.
#######################################
/edep/hitSagitta drift 1.0 mm
/edep/hitLength drift 1.0 mm
/edep/update

# Make sure that the GPS generator is it's default state.
/gps/source/clear

# Enable generating multiple particle guns per event.
/gps/source/multiplevertex true

#######################################
# The first particle source
#######################################
/gps/source/add 1

# Create the first particle source.  This source will be used to
# specify the position of the vertex.  The any of the GPS direction
# and energy commands can be used.
/gps/particle mu+
/gps/energy 700 MeV

# This generates the position of the particle gun.  It can be
# used for the other particle guns to create a multi particle event.  The
# /gps/position command must come first.
/gps/position 0.0 0.0 -0.50 m
/gps/pos/type Volume
/gps/pos/shape Para
/gps/pos/halfx 20 cm
/gps/pos/halfy 20 cm
/gps/pos/halfz 20 cm

# This generates the direction of the muon.
/gps/ang/type iso
# /gps/ang/maxtheta 15 deg
/gps/ang/rot1 1 0 0
/gps/ang/rot2 0 -1 0

#########################################
# Create the second particle source.
#########################################
/gps/source/add 2
/gps/particle proton
/gps/energy 200 MeV

# This generates the direction of the second particle gun.
/gps/ang/type iso
# /gps/ang/maxtheta 25 deg
/gps/ang/rot1 0 0 1
/gps/ang/rot2 0 1 0

# Create the GPS particle generator so it exists.
/generator/add

# Copy the vertex position of the first particle into the second particle.
/generator/combine 0 1

EOF

edep-sim -o ${OUTPUT} -C -e 100 100TestTree.mac
