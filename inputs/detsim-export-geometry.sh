#!/bin/bash
#
# Export an MC geometry to a geometry file.  This prepares a file that
# is ready to be used in the reconstruction.
#
# USAGE: ./detsim-export-geometry.sh <control-file> <1.0> ...
# 
# Make a temporary macro file.
MACRO=`mktemp -t detsim.XXXXXXXXXX` || exit 1

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

    echo /dsim/control ${CTRL} ${CTRLVERS} >> $MACRO
    shift
done

if [ x$haveControl == x ]; then
    echo Missing control file
    rm $MACRO
    exit 1
fi

OUTPUT=export-geometry${haveControl}.root
cat >> $MACRO <<EOF
# /dsim/validateGeometry
/dsim/update
/dsim/export $OUTPUT
EOF

set -e

DETSIM.exe $MACRO 

rm $MACRO

VERS=`(cd $DETSIMROOT/cmt ; cmt show version)`

strip-geom.exe -p ${VERS}${haveControl} $OUTPUT
