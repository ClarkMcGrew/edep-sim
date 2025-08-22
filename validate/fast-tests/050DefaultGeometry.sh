#!/bin/sh
#
# Generate events using a non-trivial GDML geometry.
#

OUTPUT=050DefaultGeometry.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > 050DefaultGeometry.mac <<EOF
/edep/update
/edep/export $OUTPUT
EOF

edep-sim -o ${OUTPUT} 050DefaultGeometry.mac


