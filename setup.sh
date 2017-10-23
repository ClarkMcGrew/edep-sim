#!/bin/sh
#
# Setup the edep-sim directory for development (or simply running the
# simulation).  This makes sure that the ROOT and GEANT4 environment
# variables are set (using thisroot.sh and geant4.sh), and then
# defines a couple of conveniences commands:
#
#  edep-build == Source ./build/edep-build.sh which will conveniently
#           run cmake/make/make install from any place so that it's
#           really easy to recompile.
#
#  edep-setup == Source this file.  You probably never have to use
#           this one.
#
# This setup script is not needed.  You can also do it by hand.  It's
# a usual cmake build, but you need to make sure root and geant are
# "in the path".
#
# source thisroot.sh
# source geant4.sh
# cd the-build-directory
# cmake -DCMAKE_INSTALL_PREFIX=the-install-directory the-edep-sim-directory 
# make
# make install


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

if [ ${EDEP_ROOT} = "invalid-directory" ]; then
    echo EDEP-SIM setup.sh must be sourced in edep-sim directory.
    return
fi

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

echo Defined edep-setup to re-setup the edep-sim package.
echo Defined edep-build to build the the edep-sim package.
