cmake_minimum_required(VERSION 3.30)

# Set the project name and version.  The project name will be used to
# import this into other CMAKE files.
project(EDepSim VERSION 1.0.0)

message("Energy Deposition Simulation -- ${VERSION}")

# Define the options that can be set in the cache, or on the cmake
# command line.
set(CMAKE_BUILD_TYPE Debug)
set(EDEPSIM_DISPLAY TRUE CACHE BOOL
  "If true, compile the edep-disp event display")
set(EDEPSIM_READONLY FALSE CACHE BOOL
  "If true, then DO NOT use GEANT4")
set(EDEPSIM_USE_NEST TRUE CACHE BOOL
  "If true, then make the NEST model available to be directly used")

# Check to see if this is MACOS
if(APPLE)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif (APPLE)

# Add a target to generate API documentation with Doxygen.  The build
# works fine if this is not found.
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
else(DOXYGEN_FOUND)
  add_custom_target(doc
    COMMENT "Not generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

# Make sure that ROOT is available.  ROOT is absolutely required.
find_package(ROOT REQUIRED
  COMPONENTS Geom Physics Matrix MathCore Tree RIO)
if(ROOT_FOUND)
  include(${ROOT_USE_FILE})
endif(ROOT_FOUND)

# Look for GEANT4.  If it's found, then build the simulation.
# Otherwise, this can be skipped, but only the i/o library is built.
if(NOT EDEPSIM_READONLY)
  find_package(Geant4 10.0)
  if (Geant4_FOUND)
    include(${Geant4_USE_FILE})
  else (Geant4_FOUND)
    message("GEANT not found")
    set(EDEPSIM_READONLY TRUE)
  endif (Geant4_FOUND)
endif(NOT EDEPSIM_READONLY)

# Compile the input/output classes needed to read edep-sim output.
# This does not depend on geant4.
add_subdirectory(io)

# Compile the really simple debugging display.  There are better
# displays out there, but this is what I use to debug the code.  This
# does not depend on geant4.
if(EDEPSIM_DISPLAY)
  message("Building edep-disp event display")
  add_subdirectory(display)
endif(EDEPSIM_DISPLAY)

# Only build the simulation if this is not being built in "READONLY" mode.
if(NOT EDEPSIM_READONLY)
  add_subdirectory(src)
  add_subdirectory(app)
endif(NOT EDEPSIM_READONLY)

# Install some extra files.  These are in subdirectories, but are not
# really part of the package.  They are still useful for users to have
# available as examples.

install(DIRECTORY inputs DESTINATION share/EDepSim
  FILES_MATCHING
  PATTERN README*
  PATTERN *.mac
  PATTERN *.gdml)

# install(DIRECTORY test DESTINATION share/EDepSim
#   FILES_MATCHING
#   PATTERN README*
#   PATTERN *.sh
#   PATTERN *.C
#   PATTERN *.py)

# install(DIRECTORY tools DESTINATION share/EDepSim
#   FILES_MATCHING
#   PATTERN README*
#   PATTERN *.sh
#   PATTERN *.C
#   PATTERN *.py)

#############################################################
#
# Prepare the package so that it can be used with the find_package interface.
#
#############################################################

# Include module with function 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Build the targets description so that the package can be configured
# using find_package.
install(EXPORT EDepSimTargets
  NAMESPACE EDepSim::
  DESTINATION lib/cmake/EDepSim)

# Write the 'EDepSimConfigVersion.cmake' file which can be used to
# check if a version meets the requested properties.
write_basic_package_version_file(
  EDepSimConfigVersion.cmake
  COMPATIBILITY SameMajorVersion)

# Write the 'EDepSimConfig.cmake' file so that a user package can
# access this with find_package.
configure_package_config_file(
  PackageConfig.cmake.in
  EDepSimConfig.cmake
  PATH_VARS CMAKE_INSTALL_PREFIX
  INSTALL_DESTINATION lib/cmake/EDepSim)

# Install the config files.
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/EDepSimConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/EDepSimConfigVersion.cmake
  DESTINATION lib/cmake/EDepSim )
