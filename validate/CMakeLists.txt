# Define tests.  These are run via `CTest', or by hand.  The "make
# test" target will also run ctest so the tests can be easily added to the build
#
#  This places the output of the test script in
#  ${CMAKE_BINARY_DIR}/Testing/Temporary/LastTest.log and the output
#  of the individual gundam tests in to
#  ${CMAKE_BINARY_DIR}/Testing/gundam-tests.<date>
#

message( STATUS "Setting edep-sim validation ..." )
enable_testing()

# The variable GUNDAM_TESTS_ARGUMENT may be set from the CMAKE
# (i.e. -DGUNDAM_TESTS_ARGUMENT="value") command line, or it will take
# a default value of "-f".  The useful values are "-f" for fast test,
# "-r" for regular tests, "-e" for extended tests, and "-s" for slow
# tests.
if(NOT DEFINED EDEPSIM_VALIDATE_ARGUMENT OR EDEPSIM_VALIDATE_ARGUMENT STREQUAL "")
  if(WITH_SLOW_TEST)
    set(EDEPSIM_VALIDATE_ARGUMENT "-s")
  elseif(WITH_EXTENDED_TEST)
    set(EDEPSIM_VALIDATE_ARGUMENT "-e")
  elseif(WITH_REGULAR_TEST)
    set(EDEPSIM_VALIDATE_ARGUMENT "-r")
  else()
    set(EDEPSIM_VALIDATE_ARGUMENT "-f")
  endif()
endif(NOT DEFINED EDEPSIM_VALIDATE_ARGUMENT OR EDEPSIM_VALIDATE_ARGUMENT STREQUAL "")

# Run the edep-sim-validate.sh script which runs scripted tests.  The
# COMMAND is a little complicated since the testing script needs to
# run in the source directory, but puts the output in a sub-directory
# of the binary directory.

# Run the main executable test scripts.  These are relatively slow, so they
add_test(
  NAME edep-sim-validate
  COMMAND "sh" "-c" "(cd ${CMAKE_SOURCE_DIR}/validate && ./edep-sim-validate.sh  ${EDEPSIM_VALIDATE_ARGUMENT} -a ${CMAKE_BINARY_DIR}/Testing/validate.$(date +%Y-%m-%d-%H%M%S) )"
)

