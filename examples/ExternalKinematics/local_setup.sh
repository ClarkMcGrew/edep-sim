#!/bin/bash

G4_INSTALLED_AT=~/work/projects/GEANT4/geant4-11.4.0/Debian-gcc_14-cxx17-x86_64-linux

if [ ! -f ${G4_INSTALLED_AT}/bin/geant4.sh ]; then
   echo "Make sure G4_INSTALLED_AT is set correctly"
   return
fi
   
ROOT_INSTALLED_AT=~/work/projects/ROOT/root-6.36.04/Debian-gcc_14-cxx17-x86_64-linux

if [ ! -f ${ROOT_INSTALLED_AT}/bin/thisroot.sh ]; then
   echo "Make sure ROOT_INSTALLED_AT is set correctly"
   return
fi
   
EDEPSIM_INSTALLED_AT=~/work/projects/edep-sim/devel/edep-g4.11.4.0-gcc-14-x86_64-linux-gnu

if [ ! -d ${EDEPSIM_INSTALLED_AT} ]; then
   echo "Make sure EDEPSIM_INSTALLED_AT is set correctly"
   return
fi

HERE=${PWD}

source ${G4_INSTALLED_AT}/bin/geant4.sh
cd ${HERE}

source ${ROOT_INSTALLED_AT}/bin/thisroot.sh
cd ${HERE}

___path_prepend () {
    ___path_remove $1 $2
    eval export $1="$2:\$$1"
}
___path_remove ()  {
    export $1=$(eval echo -n \$$1 | \
	awk -v RS=: -v ORS=: '$0 != "'$2'"' | \
	sed 's/:$//'); 
}

# Uncomment the next line to automatically add edep-sim to the cmake path
___path_prepend CMAKE_PREFIX_PATH ${EDEPSIM_INSTALLED_AT}

echo CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
                         
unset -f ___path_prepend
unset -f ___path_remove

export EMACS_COMPILE_COMMAND
EMACS_COMPILE_COMMAND="make -C ${PWD}/build"

