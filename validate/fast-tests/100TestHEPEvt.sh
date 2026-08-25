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
BASE=100TestHEPEvt

OUTPUT=${BASE}.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

INPUT=${BASE}.txt
cat > ${INPUT} <<EOF
         5
   1        13    0    0 -0.21729988e+02 -0.16027278e+02  0.67855399e+02  0.10566000e+00
   1        22    0    0  0.25382032e-04  0.42110624e-03  0.14112842e-04  0.00000000e+00
   1       -13    0    0 -0.86631189e+01  0.25138860e+02 -0.19823125e+02  0.10566000e+00
   1        22    0    0 -0.98220416e-01  0.29174831e+00 -0.23147411e+00  0.00000000e+00
   1        11    0    0  0.18675520e+01 -0.14315622e+02  0.24412754e+02  0.51000000e-03
         5
   1        13    0    0 -0.21729988e+02 -0.16027278e+02  0.67855399e+02  0.10566000e+00
   1        22    0    0  0.25382032e-04  0.42110624e-03  0.14112842e-04  0.00000000e+00
   1       -13    0    0 -0.86631189e+01  0.25138860e+02 -0.19823125e+02  0.10566000e+00
   1        22    0    0 -0.98220416e-01  0.29174831e+00 -0.23147411e+00  0.00000000e+00
   1        11    0    0  0.18675520e+01 -0.14315622e+02  0.24412754e+02  0.51000000e-03
EOF

MACRO=${BASE}.mac
cat > ${MACRO} <<EOF
#######################################
# Set the hit segment.
#######################################
/edep/hitSagitta drift 1.0 mm
/edep/hitSeparation drift -1.0 mm
/edep/hitLength drift 1.0 mm
/edep/update

/generator/kinematics/hepevt/input ${INPUT}
/generator/kinematics/hepevt/verbose 2
/generator/kinematics/set hepevt

/generator/count/fixed/number 1
/generator/count/set fixed
/generator/add
EOF

edep-sim -o ${OUTPUT} -C -e 3 ${MACRO} || exit 1
