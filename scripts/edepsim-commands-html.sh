#!/bin/sh

DIR=`pwd`/html

if [ ! -x ${DIR} ]; then
    mkdir ${DIR}
fi

cd ${DIR}

MACRO=`mktemp -t edepsim.XXXXXXXXXX` || exit 1
cat >> ${MACRO} <<EOF
/control/createHTML /
EOF

edep-sim $MACRO 

rm $MACRO
