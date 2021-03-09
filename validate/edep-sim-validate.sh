#!/bin/bash
#
# Run tests on edep-sim.  The tests kept in the fast-tests
# subdirectory will be run.  Any tests that take a lot of time and are
# for more detailed validation should be kept in slow-tests.  The
# slow-tests will only be run if the "-s" option is provided.
#
# This needs to be run in the validate subdirectory.

TESTS="fast-tests"

# Handle any input arguments
TEMP=$(getopt -o 's' -n "$0" -- "$@")
if [ $? -ne 0 ]; then
    echo "Error ..."
    exit 1
fi
eval set -- "$TEMP"
unset TEMP
while true; do
    case "$1" in
	'-s')
	    TESTS="${TESTS} slow-tests"
	    shift
	    continue;;
	'--')
	    shift
	    break;
    esac
done

# Make a temporary macro file in the local directory.
OUTPUT_DIR="output.$(date +%Y-%m-%d-%H%M)"
mkdir ${OUTPUT_DIR}
for i in $(find ${TESTS} -name "[0-9]*" -type f | sort); do
    if [ -x ${i} ]; then
        LOG=$(basename $i)
        echo "(cd $OUTPUT_DIR && ../${i} >& ${LOG}.log || echo FAIL: $i)"
        (cd $OUTPUT_DIR && ../${i} >& ${LOG}.log || echo FAIL: $i)
    fi
done
        
