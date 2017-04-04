#!/bin/sh

# Try to setup root.  ROOT installs thisroot.sh in the bin directory
# to setup the environment.  The first "thisroot.sh" in the path will
# define the root that is used.
. thisroot.sh >& /dev/null
if [ $? != 0 ]; then
    echo ROOT not available.
fi

# Try to setup geant4.  GEANT4 installs geant4.sh in the bin directory
# to setup the environment.  The first "geant4.sh" in the path will
# define the geant that is used.
. geant4.sh >& /dev/null
if [ $? != 0 ]; then
    echo GEANT not available.
fi

# Find the root of the building area.
___edep_root() {
    COUNT=50
    while true; do
	if [ -e ./build -a -d ./build -a -e ./build/edep-build.sh ]; then
	    echo ${PWD}
	    return
	fi
	COUNT=$(expr ${COUNT} - 1)
	if [ ${COUNT} -lt 1 ]; then
	    echo invalid-directory
	    return
	fi
	cd ..
    done
}
export EDEP_ROOT
EDEP_ROOT=$(___edep_root)
unset -f ___edep_root

___edep_target () {
    target="edep"
    compiler=gcc
    target="${target}-${compiler}-$(cc -dumpversion)-$(cc -dumpmachine)"
    echo $target
}

export EDEP_TARGET
EDEP_TARGET=$(___edep_target)
unset -f ___edep_target

___path_prepend () {
    ___path_remove $1 $2
    eval export $1="$2:\$$1"
}
___path_remove ()  {
    export $1=$(eval echo -n \$$1 | \
	awk -v RS=: -v ORS=: '$0 != "'$2'"' | \
	sed 's/:$//'); 
}

___path_prepend PATH ${EDEP_ROOT}/${EDEP_TARGET}/bin
___path_prepend LD_LIBRARY_PATH ${EDEP_ROOT}/${EDEP_TARGET}/lib

unset -f ___path_prepend
unset -f ___path_remove


alias edep-setup=". ${EDEP_ROOT}/setup.sh"

alias edep-build="${EDEP_ROOT}/build/edep-build.sh"

echo Defined edep-setup to re-setup the edep-sim package
echo Define edep-build to build the the edep-sim package.
