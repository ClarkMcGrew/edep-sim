#!/bin/sh
#
# Write a ROOT file in the default geometry.  This output file should
# be tested to make sure that all of the fields are readable.

# Get the directory containing the script from the command line
# parameters (avoids bash trickery).  Use the current directory as the
# default.
DIR=.
if [ ${#1} -gt 0 ]; then
    DIR=${1}
fi

# Make sure that edep-sim has been setup.
if ! which edep-sim; then
    echo FAIL: Executable not found for edep-sim
    exit 1
fi

# Setup the base names 
BASE=101FailHEPEvt

OUTPUT=${BASE}.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

INPUT=${BASE}.txt
cat > ${INPUT} <<EOF
1.0 13.0 0.0 0.0 0.0 0.0 1.0392633396995 0.09623568146043286 0.3492655213021866 1.105658 0.105658 482.8700598805462 331.1324933745929 300.63208929581367 2.9219691707960083
EOF

MACRO=${BASE}.mac
cat > ${MACRO} <<EOF
#######################################
# Set the hit segment.
#######################################
/edep/hitSagitta drift 1.0 mm
/edep/hitLength drift 1.0 mm
/edep/update

/generator/kinematics/hepevt/input ${INPUT}
/generator/kinematics/hepevt/verbose 2
/generator/kinematics/set hepevt

/generator/count/fixed/number 1
/generator/count/set fixed
/generator/add
EOF

edep-sim -o ${OUTPUT} -C -e 3 ${MACRO} | tee ${BASE}.output 

grep "ERROR:.*EDepSimHEPEVT.*Syntax error" ${BASE}.output || exit 1

echo SUCCESS
