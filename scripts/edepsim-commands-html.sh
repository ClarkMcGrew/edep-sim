#!/bin/sh
##
## A standalone script to build the geant4 command documentation in
## the HTML format.
##
CUR=$(pwd)
DIR=$(pwd)/html

if [ ! -x ${DIR} ]; then
    mkdir ${DIR}
fi

cd ${DIR}

MACRO=$(mktemp -t edepsim.XXXXXXXXXX) || exit 1
cat >> ${MACRO} <<EOF
/control/createHTML /
EOF

edep-sim $MACRO 

rm $MACRO

cd ${CUR}

if [ -x ../doc/html ]; then
   echo "PLEASE REMOVE ../doc/html"
   exit 1
fi

mv ./html ../doc

