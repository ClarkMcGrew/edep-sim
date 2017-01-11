#!/bin/bash
if [ ${#EDEP_ROOT} == 0 ]; then
    echo EDEP is not setup yet.  You need to source build/edep-setup.sh.
    echo You can also run cmake and make by hand...
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

if [ ${#1} != 0 -a "x${1}" == "xforce" ]; then
    shift
    echo Reconfigure build.
    if [ -f  CMakeCache.txt ]; then
	rm CMakeCache.txt
    fi
    if [ -d CMakeFiles ]; then
	rm -rf CMakeFiles
    fi
fi

if [ ! -f CMakeCache.txt ]; then
    cmake -DCMAKE_INSTALL_PREFIX=${EDEP_ROOT}/${EDEP_TARGET} ${EDEP_ROOT}
fi

make $*
make install



