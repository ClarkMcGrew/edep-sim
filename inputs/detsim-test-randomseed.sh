#!/bin/sh
# 
# Test that the random seed macro commands are working.

source ${DETSIMROOT}/cmt/setup.sh

MACRO=`mktemp -t detsim.XXXXXXXXXX` || exit 1
cat >> $MACRO <<EOF
/dsim/control baseline 1.0
/dsim/random/showRandomSeed
/dsim/random/timeRandomSeed
/dsim/random/showRandomSeed
/dsim/random/randomSeed 20090401
/dsim/random/showRandomSeed
/dsim/update

/gps/direction 0 0 1
/gps/position -100 -100 -638.814 mm
/gps/verbose 0
/gps/particle mu+
/gps/energy 254.17 MeV
/run/beamOn 5

/dsim/random/showRandomSeed
EOF

OUTPUT=detsim-randomseed

if [ -f $OUTPUT ]; then
    rm $OUTPUT
fi

DETSIM.exe -o $OUTPUT $MACRO 

rm $MACRO
