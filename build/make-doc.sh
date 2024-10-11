#!/bin/bash
##
## A script to build edep-sim documentation.  This can build from
## "anyplace". (for some definition of "anyplace", don't test that too
## agressively).  This will build a pdf file for the README, and
## rebuild the GEANT4 command documentation
## ("doc/edepsim-command.list").
##

if [ ${#EDEP_ROOT} == 0 ]; then
    echo EDEPSIM is not setup yet.  You need to source build/edep-setup.sh.
    exit 1
fi

BUILD_LOCATION=${EDEP_ROOT}/${EDEP_TARGET}
if [ ! -d ${BUILD_LOCATION} ]; then
    mkdir -p ${BUILD_LOCATION}
fi

if [ ! -d ${BUILD_LOCATION} ]; then
    echo Unable to access build location at ${BUILD_LOCATION}
    exit 1
fi

cd ${BUILD_LOCATION}

if [ -x ./bin/edep-sim &> /dev/null ] ; then
    echo BUILD THE COMMAND LIST

    ####################################################
    # Make the text file and put it in "doc"
    ####################################################

    MACRO=`mktemp -t edepsim.XXXXXXXXXX` || exit 1
    TFILE=`mktemp -t edepsim.XXXXXXXXXX` || exit 1
    echo /control/manual >> ${MACRO}

    edep-sim $MACRO > ${TFILE}

    LINE=$(grep -n "^Command directory path : /$" ${TFILE} | grep -o '[0-9]*')

    tail -n +${LINE} ${TFILE} > edepsim-command.list

    rm ${TFILE}
    rm ${MACRO}

    mv edepsim-command.list ../doc

    ####################################################
    # Make the html files
    ####################################################

    # Uncomment this to build the HTML.  It's works, but I'm biased
    # and don't find the output useful.  There is a standalone script
    # that will build this for "personal use".
    
    # DIR=$(pwd)/html
    # if [ ! -x ${DIR} ]; then
    #     mkdir ${DIR}
    # fi
    # cd ${DIR}
    # rm *.html
    # MACRO=`mktemp -t edepsim.XXXXXXXXXX` || exit 1
    # echo /control/createHTML >> ${MACRO}
    # edep-sim $MACRO
    # rm ${MACRO}

fi

if which pandoc &> /dev/null ; then
    echo BUILD THE README PDF
    pandoc --variable=margin-left:2.5cm \
           --variable=margin-right:2.5cm \
           --variable=margin-top:2.5cm \
           --variable=margin-bottom:2.5cm \
           ${EDEP_ROOT}/README.md \
           -o ${EDEP_ROOT}/README.pdf
fi
