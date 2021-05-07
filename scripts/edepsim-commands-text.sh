#!/bin/bash
## A deprecated script to build the text documentation.  Use the
## "build/make-doc.sh" script instead.

MACRO=`mktemp -t edepsim.XXXXXXXXXX` || exit 1
cat >> $MACRO <<EOF
/control/manual
EOF

edep-sim $MACRO > edepsim-command.temp

LINE=`grep -n "^Command directory path : /$" edepsim-command.temp | grep -o '[0-9]*'`

tail -n +${LINE} edepsim-command.temp > edepsim-command.list

rm edepsim-command.temp
rm $MACRO

mv edepsim-command.list ../doc
