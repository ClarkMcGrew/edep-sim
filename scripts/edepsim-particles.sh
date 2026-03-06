#!/bin/bash
## A deprecated script to build the text documentation.  Use the
## "build/make-doc.sh" script instead.

MACRO=`mktemp -t edepsim.XXXXXXXXXX` || exit 1
cat >> $MACRO <<EOF
/gps/List
EOF

edep-sim $MACRO > edepsim-particles.temp

LINE=`grep -n "^/gps/List" edepsim-particles.temp | grep -o '[0-9]*'`

tail -n +${LINE} edepsim-particles.temp | grep -v '^ERROR' > edepsim-particles.list

rm edepsim-particles.temp
rm $MACRO

mv edepsim-particles.list ../doc
