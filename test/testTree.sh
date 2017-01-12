#!/bin/sh

if [ -f edepsimTree.root ]; then
    rm edepsimTree.root
fi

if [ -d edepsimTree ]; then
    rm -rf edepsimTree
fi

edep-sim ${EDEP_ROOT}/inputs/muon-100.mac -o edepsimTree

root -q edepsimTree.root makeTree.C readTree.C++

   
