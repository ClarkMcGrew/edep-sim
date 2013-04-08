#!/bin/sh

source ${DETSIMROOT}/cmt/setup.sh

DIR=`pwd`/html

if [ ! -x ${DIR} ]; then
    mkdir ${DIR}
fi

cd ${DIR}

MACRO=`mktemp -t detsim.XXXXXXXXXX` || exit 1
cat >> ${MACRO} <<EOF
/control/createHTML /
EOF

DETSIM.exe $MACRO 

rm $MACRO
