#!/bin/bash
#
# Run test scripts on the full edep-sim executable. The tests scripts
# are run in order so that the output of early tests is available for
# later tests.
#
# Test scripts that are kept in the fast-tests subdirectory will
# always be run.  Any test scripts that take a lot of time and are for
# more detailed validation should be kept in either regular-tests,
# extended-tests or slow-tests.  Tests that are not part of
# "fast-tests" will only be run when the applicable options are set.
# The apply option ("-a") must be added to actually run the scripts.
#
# The testing levels are:
#
#    fast-tests/ -- Always run and used during continuous integration.
#
#    regular-tests/ -- Quick tests that are not used for CI, but
#       should be run locally before a push/pull-request Run when "-r"
#       is provided.  They are run after and can use results from the
#       fast-tests.  (Plan to get a dring of water while these tests run).
#
#    extended-tests/ -- Slower tests that are run when "-e" is
#       provided.  These tests should finish in well under 30 seconds,
#       and all of the tests should take less than a few minutes.
#       They are run after and can use results from the fast and
#       regular tests. (Plan to take a coffee break while these tests
#       run).
#
#    slow-tests/ -- Long validation tests.  Only run with "-s" is
#       provided.  These tests are run after all other tests are
#       finished. (Plan to work on something else while these tests
#       run).
#
# This needs to be run in the tests subdirectory (which contains this
# script).  Any tests that are expected to fail should be listed in
# the EXPECTED_FAILURES file (by file name relative to the tests
# directory) where there is an example called
# "fast-tests/090ExpectedFailure.sh" that is part of the testing
# framework.
#
# Validation scripts can be any executable file, but are generally
# written in bash or python.  They are run in a separate execution
# directory with command line
#
# cd <output> && <script> <directory>
#
# Where <output> is directory where the script is run, <script> is the
# full path of the test script, and <directory> is the full path of
# the directory containing the test script.  Any necessary
# configuration files should be saved in the same directory as the
# script.
#
# The edep-sim-validate.sh script will run all of the executable scripts in
# the script directories (i.e. fast-tests and/or slow-tests) that
# start with a digit.  The list of scripts to be run are printed
# before they start to run.  All of the fast-tests are run before all
# of the slow-tests (i.e. slow-tests can use output from fast-tests)
#
# The validation scripts are run in the order of increasing speed, so
# fast-tests are run before slow-tests.  Tests in a particular
# category (e.g. fast-tests) are run in lexical order based on the
# script name.  This means that script "001MyName" is run before
# "002MyName", so users have controll of the script order. The
# following convention is suggested for script naming.
#
#    000-099 -- Reserved for edep-sim-validate.sh.  This is where job
#               headers and similar things can be generated.
#
#    100-199 -- Scripts which don't require input.  This includes any
#               scripts generating input data that can be used by the
#               later tests.
#
#    200-299 -- Scripts which generate edepsim output files.  These
#               scripts mostly apply fits.
#
#    800-899 -- Scripts which produce summary files.
#
#    900-998 -- Scripts looking at summary files and checking results
#
#    999 -- Reserved for edep-sim-validate.sh.  This is where job
#               completion information is generated.
#
# NAMING CONVENTION EXAMPLE: This is how the naming convention works
# in practice.  This is how a script that runs a EDEPSIM fit that takes
# a macro file might be named.
#
#   fast-test/
#     200RunEDEPSIM.sh          -- The script
#     200RunEDEPSIM-inputs.mac  -- The macro file file (if it exists
#
#   The output file should be named 200RunEDEPSIM.root (or similar as
#   needed).

echo 'USAGE: edep-sim-validate.sh [-f] [-r] [-e] [-s] [-a] [output-directory]'
echo '    -c               : Force use of terminfo colors for output'
echo '    -f               : Only run the fast tests [default]'
echo '    -r               : Run fast and regular tests'
echo '    -e               : Run fast, regular and extended tests'
echo '    -s               : Run all tests including the slow tests'
echo '    -a               : Apply the tests (no tests are run without this)'
echo '    output-directory : The name of the output directory.  The default'
echo '                       value is \"./output.YYYY-MM-DD-hhmm\"'
echo ' See edep-sim-validate.sh for more usage documentation.'

# The default tests to be run.
TESTS="fast-tests"

# Handle any input arguments
if [ $(uname) == "Darwin" ]; then
    # Work around MacOS bug
    TEMP=$(getopt "$0" "$@")
else
    TEMP=$(getopt -o 'acfres' -n "$0" -- "$@")
fi

# Check for getopt errors
if [ $? -ne 0 ]; then
    exit 1
fi

eval set -- "$TEMP"
unset TEMP
while true; do
    case "$1" in
        '-c') # Force colors
            USE_COLORS="yes"
            shift
            continue;;
	'-f')
            # be explicit about the testing
	    TESTS="fast-tests"
	    shift
	    continue;;
	'-r')
            # be explicit about the testing
	    TESTS="fast-tests regular-tests"
	    shift
	    continue;;
	'-e')
            # be explicit about the testing
	    TESTS="fast-tests regular-tests extended-tests"
	    shift
	    continue;;
	'-s')
            # be explicit about the testing
	    TESTS="fast-tests regular-tests extended-tests slow-tests"
	    shift
	    continue;;
        '-a')
            APPLY="yes"
            shift
            continue;;
	'--')
	    shift
	    break;
    esac
done

echo
echo Requesting tests in ${TESTS}

# Result when this script has a problem.
RESULT_ERROR="ERROR:"
RESULT_WARNING="WARNING:"

# Result for a particular sub job.
RESULT_JOB_FAILURE="JOB FAILURE:"
RESULT_JOB_SUCCESS="JOB SUCCESS:"

# Result for the test.  These report the result to the testing harness.
RESULT_FAILURE="FAIL:"
RESULT_SUCCESS="SUCCESS:"

# Add colors to the results (on terminals only)
if [ -t 1 -o ${USE_COLORS}x == "yesx" ]; then
    TERMINFO_INIT=$(tput init)
    TERMINFO_RED=$(tput setaf 1)
    TERMINFO_YELLOW=$(tput setaf 3)
    TERMINFO_GREEN=$(tput setaf 2)
    RESULT_ERROR=${TERMINFO_RED}${RESULT_ERROR}${TERMINFO_INIT}
    RESULT_WARNING=${TERMINFO_YELLOW}${RESULT_ERROR}${TERMINFO_INIT}
    RESULT_JOB_FAILURE=${TERMINFO_RED}${RESULT_JOB_FAILURE}${TERMINFO_INIT}
    RESULT_JOB_SUCCESS=${TERMINFO_GREEN}${RESULT_JOB_SUCCESS}${TERMINFO_INIT}
    RESULT_FAILURE=${TERMINFO_RED}${RESULT_FAILURE}${TERMINFO_INIT}
    RESULT_SUCCESS=${TERMINFO_GREEN}${RESULT_SUCCESS}${TERMINFO_INIT}
fi

# Find the name of the output directory.  It might have been provided
# on the command line.
OUTPUT_DIR="output.$(date +%Y-%m-%d-%H%M)"  # A default name for the output
if [ ${#1} -gt 0 ]; then
    # A name was provided on the command line.
    OUTPUT_DIR=${1}
fi

echo Output will be in ${OUTPUT_DIR}

# Make sure the output directory does not exist.
if [ -x ${OUTPUT_DIR} ]; then
    echo -e ${RESULT_ERROR} Output directory already exists ${OUTPUT_DIR}
    exit 1
fi

echo Running in ${PWD}
if [ ! -x ./edep-sim-validate.sh ]; then
    echo -e ${RESULT_ERROR} Must be run from the directory containing edep-sim-validate.sh
    exit 1
fi

for i in ${TESTS}; do
    if [ -x ${PWD}/${i} ]; then
        echo Testing directory found: $i
        for j in $(find ${i} -name "[0-9]*" -type f | grep -v "~" | sort); do
            if [ -x ${j} ]; then
                echo '   Will run:' $j
            fi
        done
    fi
done

if [ ! -f EXPECTED_FAILURES ]; then
    echo -e ${RESULT_ERROR} EXPECTED_FAILURES file must exist, but it can be empty.
    exit 1
fi

if [ ${APPLY}x != "yesx" ]; then
    echo
    echo -e ${RESULT_ERROR} Tests not run. Add the -a option to run the test.
    exit 1
fi


###################################################################
#
# Start the actual testing.
#
###################################################################

# Make sure the output directory has been created
mkdir -p ${OUTPUT_DIR}

# Make sure the output directory was correctly created (i.e. it exists)
if [ ! -x ${OUTPUT_DIR} ]; then
    echo -e ${RESULT_ERROR} OUTPUT DIRECTORY WAS NOT CREATED: ${OUTPUT_DIR}
    exit 1
fi

# Find and run the jobs in lexical order.
FAILURES=""
EXPECTED=""
for d in ${TESTS}; do
    if [ ! -x ${PWD}/${d} ]; then
        echo -e ${RESULT_WARNING} TESTING DIRECTORY ${d} DOES NOT EXIST
        continue;
    fi
    for i in $(find ${d} -name "[0-9]*" -type f | grep -v "~" | sort); do
        JOB=${PWD}/${i}
        # Only run files that are executable
        if [ ! -x ${JOB} ]; then
            continue;
        fi
        # SUCCESS is false by default.
        SUCCESS="no"
        # Get the full path to the script.  This is passed to the script
        # so the script can easily find any input files.
        DIR=$(dirname ${JOB})
        # The name of the output log file
        LOG=$(basename ${JOB}).log
        # Run the script in the output directory.
        echo "(cd $OUTPUT_DIR && ${JOB} ${DIR})"
        if (cd $OUTPUT_DIR && ${JOB} ${DIR} >& ${LOG}); then
            # The job exited with success, but look for a fail messsage
            if (tail -5 ${OUTPUT_DIR}/${LOG} | grep FAIL >> /dev/null); then
                echo -e ${RESULT_JOB_FAILURE} ${i}
            elif (tail -10 ${OUTPUT_DIR}/${LOG} | grep "Execution.*aborted" >> /dev/null); then
                echo -e ${RESULT_JOB_FAILURE} ${i}
            else
                echo -e ${RESULT_JOB_SUCCESS} ${i}
                SUCCESS="yes"
            fi
        else
            echo -e ${RESULT_JOB_FAILURE} ${i}
        fi
        if [ ${SUCCESS} = "yes" ]; then
            # The job succeeded, make sure it's not in EXPECTED_FAILURES
            if (grep -F $i EXPECTED_FAILURES >> /dev/null); then
                cat ${OUTPUT_DIR}/${LOG}
                echo -e ${RESULT_JOB_FAILURE} Expected $i to fail
                FAILURES="${FAILURES} unexpected-success:\"${JOB}\""
            fi
        else
            # The job failed, check if it was expected
            if (grep -F $i EXPECTED_FAILURES >> /dev/null); then
                cat ${OUTPUT_DIR}/${LOG}
                echo -e ${RESULT_JOB_SUCCESS} Failure was expected for $i
                EXPECTED="${EXPECTED} \"${JOB}\""
            else
                cat ${OUTPUT_DIR}/${LOG}
                FAILURES="${FAILURES} unexpected-failure:\"${JOB}\""
            fi
        fi
    done
done

if [ ${#EXPECTED} -gt 0 ]; then
    echo
    echo Expected Failures:
    for i in ${EXPECTED}; do
        echo EXPECTED FAILURE: $i
    done
fi

if [ ${#FAILURES} -gt 0 ]; then
    echo
    echo Failed Jobs:
    for i in ${FAILURES}; do
        echo UNEXPECTED FAILURE: $i
    done
    echo
    echo -e ${RESULT_FAILURE} Tests failed
    exit 1
else
    echo
    echo -e ${RESULT_SUCCESS} Tests succeeded
fi
# End of the script
