#!/bin/sh
#
# Generate events using a non-trivial GDML geometry.
#

BASE=060GDMLGeometry
INPUTBASE=../fast-tests/${BASE}

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


