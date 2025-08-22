#!/bin/bash
#
# Run tests on edep-sim.  The tests kept in the fast-tests
# subdirectory will be run.  Any tests that take a lot of time and are
# for more detailed validation should be kept in slow-tests.  The
# slow-tests will only be run if the "-s" option is provided.
#
# This needs to be run in the validate subdirectory.
#
# The validation scripts are run in lexical order based on the script
# name.  This means that script "001MyName" is run before "002MyName",
# and allows users to specify the script order.  This script is used
# in a few places.  The following convention is suggested for script
# naming.
#
#    000     -- Reserved for edep-sim-validate.sh
#
#    001-099 -- Scripts which don't require input.  This includes any
#        scripts generating kinematic output that could be used by
#        edep-sim.
#
#    100-199 -- Scripts which generate edep-sim output files.
#
#    200-299 -- Scripts which produce "raw" hits (and might need
#        edep-sim input files).  This is where the detector response
#        simulation is expected to be run.  These scripts might also
#        produce calibrated hits.
#
#    300-399 -- Scripts which need uncalibrated hit objects.  This is
#        where scripts that produce calibration coefficients are
#        expected to be found.
#
#    400-499 -- Scripts which produce calibrate uncalibrated hits
#        into calibrated hits.
#
#    500-599 -- Scripts which need calibrated objects.  This is
#        where the reconstruction tests should be run.
#
#    600-699 -- Scripts which need reconstruction objects.
#
#    800-899 -- Scripts which produce summary trees.
#
#    900-999 -- Scripts looking at summary trees and validating physics
#        performance.


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
for i in $(find ${TESTS} -name "[0-9]*" -type f | grep -v "~" | sort); do
    if [ -x ${i} ]; then
        LOG=$(basename $i)
        echo "(cd $OUTPUT_DIR && ../${i} >& ${LOG}.log || echo FAIL: $i)"
        (cd $OUTPUT_DIR && ../${i} >& ${LOG}.log || echo FAIL: $i)
    fi
done
        
