cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

# Set the project name and version
project(edep-io)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("Energy Deposition Display -- ${VERSION}")

# Set the default verbosity of the compilation...
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# Configure the dependencies
find_package(ROOT REQUIRED
  COMPONENTS Geom Physics Matrix MathCore Tree RIO) 
if(ROOT_FOUND)
  include(${ROOT_USE_FILE})
endif(ROOT_FOUND)

# Suck up all of the .cc files for the source.  This isn't good CMAKE
# practice, but it makes maintaining this file easier.
set(source
  TG4PrimaryVertex.cxx
  TG4Trajectory.cxx
  TG4HitSegment.cxx
  TG4Event.cxx)

set(includes
  TG4PrimaryVertex.h
  TG4Trajectory.h
  TG4HitSegment.h
  TG4Event.h)

# Make sure the current directories are available for the later compilation
include_directories(${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../io)

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__edepsim_io
  TG4PrimaryVertex.h TG4Trajectory.h TG4HitSegment.h TG4Event.h
  OPTIONS -inlineInputHeader
  LINKDEF edepsim_io_LinkDef.h)

# Build the library.
add_library(edepsim_io SHARED ${source} G__edepsim_io.cxx)
target_include_directories(edepsim_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edepsim_io PUBLIC ${ROOT_LIBRARIES})

# Install the library for edep-sim
install(TARGETS edepsim_io LIBRARY DESTINATION lib)

# Install the header files.
install(FILES ${includes} DESTINATION include/edep-sim)

# If this is ROOT6 or later, then install the rootmap and pcm files.
if(${ROOT_VERSION} VERSION_GREATER 6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim_io.rootmap
    DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim_io_rdict.pcm
    DESTINATION lib)
endif(${ROOT_VERSION} VERSION_GREATER 6)
