message("Energy Deposition Display")

# Set the default verbosity of the compilation...
# set(CMAKE_VERBOSE_MAKEFILE TRUE)

# Configure the dependencies
find_package(ROOT REQUIRED
  COMPONENTS Geom Physics Matrix MathCore Tree RIO) 
if(ROOT_FOUND)
  include(${ROOT_USE_FILE})
endif(ROOT_FOUND)

# Define the source that should be used for the io part of edep-sim.
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

# Compile the base library with private I/O fields.
add_definitions(-DEDEPSIM_FORCE_PRIVATE_FIELDS)

# Make sure the current directories are available for the later
# compilation.  This is required to make ROOT_GENERATE_DICTIONARY
# work.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__edepsim_io
  TG4PrimaryVertex.h TG4Trajectory.h TG4HitSegment.h TG4Event.h
  OPTIONS -inlineInputHeader
  LINKDEF edepsim_io_LinkDef.h)

# Build the library.
add_library(edepsim_io SHARED ${source} G__edepsim_io.cxx)

target_include_directories(edepsim_io PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:include/EDepSim>")

target_link_libraries(edepsim_io PUBLIC ${ROOT_LIBRARIES})

# Install the library for edep-sim
install(TARGETS edepsim_io
  EXPORT EDepSimTargets
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/EDepSim )

# Install the header files.
install(FILES ${includes} DESTINATION include/EDepSim)

# If this is ROOT6 or later, then install the rootmap and pcm files.
if(${ROOT_VERSION} VERSION_GREATER 6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim_io.rootmap
    DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim_io_rdict.pcm
    DESTINATION lib)
endif(${ROOT_VERSION} VERSION_GREATER 6)

