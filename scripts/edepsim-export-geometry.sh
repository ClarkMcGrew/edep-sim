#!/bin/bash
#
# Export an MC geometry to a geometry file.  This prepares a file that
# is ready to be used in the reconstruction.
#
# USAGE: ./edepsim-export-geometry.sh <control-file> <1.0> ...
# 
# Make a temporary macro file.
MACRO=`mktemp -t edepsim.XXXXXXXXXX` || exit 1

haveControl=""

# write the control lines.
while true; do
    CTRL=$1
    if [ x$CTRL == x ]; then
	break;
    fi

    shift
    CTRLVERS=$1
    if [ x$CTRLVERS == x ]; then
	echo Missing control file
	rm $MACRO
	exit 1
    fi

    haveControl=${haveControl}"-"${CTRL}

    echo /edep/control ${CTRL} ${CTRLVERS} >> $MACRO
    shift
done

if [ x$haveControl == x ]; then
    echo Missing control file
    rm $MACRO
    exit 1
fi

OUTPUT=export-geometry${haveControl}.root
cat >> $MACRO <<EOF
# /edep/validateGeometry
/edep/update
/edep/export $OUTPUT
EOF

set -e

edep-sim -C $MACRO 

rm $MACRO

