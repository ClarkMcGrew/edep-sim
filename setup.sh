#!/bin/sh

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

unset -f ___path_prepend
unset -f ___path_remove


alias edep-setup=". ${EDEP_ROOT}/setup.sh"

alias edep-build="${EDEP_ROOT}/build/edep-build.sh"

echo Defined edep-setup to re-setup the edep-sim package
echo Define edep-build to build the the edep-sim package.

if which geant4.sh >> /dev/null; then
    . geant4.sh
else
    echo GEANT not available.
fi

