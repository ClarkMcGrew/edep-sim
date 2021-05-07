#!/bin/sh

OUTPUT=testGGD.gdml

if ! which gegede-cli >& /dev/null ; then
    echo The executable gegede-cli must be in your path.
fi

PYTHONPATH=$(pwd):${PYTHONPATH} gegede-cli -o ${OUTPUT} ggdExample.cfg
