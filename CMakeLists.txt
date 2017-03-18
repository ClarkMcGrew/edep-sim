cmake_minimum_required(VERSION 3.0)

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
find_package(ROOT REQUIRED COMPONENTS Geom)
include(${ROOT_USE_FILE})

# Configure GEANT4
find_package(Geant4)
include(${Geant4_USE_FILE})

# Add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

add_definitions(-DDETSIM_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

# Build the subdirectories
add_subdirectory(src)
add_subdirectory(app)

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
