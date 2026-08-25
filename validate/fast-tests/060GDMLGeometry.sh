#!/bin/sh
#
# Generate events using a non-trivial GDML geometry.
#

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

# Setup where base names 
BASE=060GDMLGeometry
INPUTBASE=${DIR}/${BASE}

GDML=${INPUTBASE}.gdml

OUTPUT=${BASE}.root
MACRO=${BASE}.mac

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > ${MACRO} <<EOF
/edep/update
/edep/export $OUTPUT
EOF

edep-sim -o ${OUTPUT} -g ${GDML} ${MACRO}

# End of test
