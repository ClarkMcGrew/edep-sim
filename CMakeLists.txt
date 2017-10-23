cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

# Set the project name and version
project(edep-sim)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("Energy Deposition Simulation -- ${VERSION}")

# Set the default verbosity of the compilation...
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# Configure the dependencies
find_package(ROOT REQUIRED
  COMPONENTS Geom Physics Matrix MathCore Tree RIO)
if(ROOT_FOUND)
  include(${ROOT_USE_FILE})
endif(ROOT_FOUND)

# Add a target to generate API documentation with Doxygen
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

# Configure GEANT4.  If it's found, then build the simulation.
find_package(Geant4)
if(Geant4_FOUND)
  include(${Geant4_USE_FILE})
  # Make sure we have at least G4.9.6
  if(${Geant4_VERSION} VERSION_LESS "9.6")
    message(FATAL_ERROR "Incompatible version of geant4")
  endif(${Geant4_VERSION} VERSION_LESS "9.6")
  add_subdirectory(src)
  add_subdirectory(app)
endif(Geant4_FOUND)
  
add_subdirectory(display)

# Install some extra files.
install(DIRECTORY inputs DESTINATION share/EDepSim
  FILES_MATCHING
  PATTERN README*
  PATTERN *.mac
  PATTERN *.gdml)
install(DIRECTORY test DESTINATION share/EDepSim
  FILES_MATCHING
  PATTERN README*
  PATTERN *.sh
  PATTERN *.C
  PATTERN *.py)
install(DIRECTORY tools DESTINATION share/EDepSim
  FILES_MATCHING
  PATTERN README*
  PATTERN *.sh
  PATTERN *.C
  PATTERN *.py)
