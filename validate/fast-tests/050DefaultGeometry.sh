#!/bin/sh
#
# Export the default geometry. NOTE! The default geometry has known
# overlaps, but they are accepted.
#

OUTPUT=050DefaultGeometry.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

cat > 050DefaultGeometry.mac <<EOF
/edep/update
/edep/export $OUTPUT
EOF

edep-sim -C -o ${OUTPUT} 050DefaultGeometry.mac


