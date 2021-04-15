#!/bin/sh
#
# Generate events using a non-trivial GDML geometry.
#

BASE=../fast-tests/060GDMLGeometry
OUTPUT=${BASE}.root
MACRO=${BASE}.mac
GDML=${BASE}.gdml

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > ${MACRO} <<EOF
/edep/update
/edep/export $OUTPUT
EOF

edep-sim -o ${OUTPUT} -g ${GDML} ${MACRO}


