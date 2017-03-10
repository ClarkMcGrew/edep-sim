#!/bin/sh

if [ -f edepsimTree.root ]; then
    rm edepsimTree.root
fi

if [ -d edepsimTree ]; then
    rm -rf edepsimTree
fi

edep-sim -o edepsimTree.root \
         ${EDEP_ROOT}/inputs/update.mac \
	 ${EDEP_ROOT}/inputs/muon-200.mac \
	 ${EDEP_ROOT}/inputs/beam-10.mac \

root -l -q edepsimTree.root ${EDEP_ROOT}/tools/loadEDepSim.C readTree.C

   
