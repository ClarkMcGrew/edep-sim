#!/bin/bash

#!/bin/sh

OUTPUT=testEvents.root

if [ -f ${OUTPUT} ]; then
    rm ${OUTPUT}
fi

MACRO=$(mktemp -t detsim.XXXXXXXXXX) || exit 1
cat > ${MACRO} <<EOF
/gps/particle mu+
/gps/energy 100 MeV
/gps/position 0.0 0.0 -15.0 cm
/gps/pos/type Volume
/gps/ang/type iso
EOF

edep-sim -o ${OUTPUT} -C -e 100 -u ${MACRO}

rm ${macro}
