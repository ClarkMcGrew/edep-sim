#!/bin/sh
#
# Generate events using a non-trivial GDML geometry.
#

OUTPUT=100TestGDML.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > 100TestGDML.mac <<EOF
# Create the first particle source.  This source will be used to
# specify the position of the vertex.  The any of the GPS direction
# and energy commands can be used.
/gps/particle mu+
/gps/energy 10000 MeV

# This generates the position of the particle gun.  It can be
# used for the other particle guns to create a multi particle event.  The
# /gps/position command must come first.
/gps/position 0.0 0.0 0.0 m
/gps/pos/type Volume
/gps/pos/shape Para
/gps/pos/halfx 50 cm
/gps/pos/halfy 50 cm
/gps/pos/halfz 50 cm

# This generates the direction of the muon.  This aims
# the particle downstream with the directions uniformly distributed in
# a 10 deg cone around the Z axis.
/gps/ang/type iso
/gps/ang/rot1 1 0 0 
/gps/ang/rot2 0 -1 0

/generator/count/fixed/number 300
/generator/count/set fixed

/generator/add
EOF

edep-sim -o ${OUTPUT} -g ${EDEP_ROOT}/inputs/example.gdml \
	 -e 1 -u 100TestGDML.mac


