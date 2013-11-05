#!/bin/bash

source ${DETSIMROOT}/cmt/setup.sh

MACRO=`mktemp -t detsim.XXXXXXXXXX` || exit 1
cat >> $MACRO <<EOF
/control/manual
EOF

DETSIM.exe $MACRO > detsim-command.temp

LINE=`grep -n "^Command directory path : /$" detsim-command.temp | grep -o '[0-9]*'`

tail -n +${LINE} detsim-command.temp > detsim-command.list

rm detsim-command.temp
rm $MACRO

cp detsim-command.list ${DETSIMROOT}/doc
