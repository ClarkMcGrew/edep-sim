# Suck up all of the .cc files for the source.  This isn't good CMAKE
# practice, but it makes maintaining this file easier.
file(GLOB_RECURSE source *.cc)
file(GLOB_RECURSE includes EDepSim*.hh TG4*.h)

# Make sure the current directories are available for the root
# dictionary generation.
include_directories(${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__edepsim
  TG4PrimaryVertex.h TG4Trajectory.h TG4HitSegment.h TG4Event.h
  OPTIONS -inlineInputHeader
  LINKDEF edepsim_LinkDef.h)

# Where the macro files can be found.
add_definitions(-DDETSIM_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

# Build the library.
add_library(edepsim SHARED ${source}
  TG4PrimaryVertex.cxx  TG4Trajectory.cxx TG4HitSegment.cxx TG4Event.cxx
  G__edepsim.cxx)
target_include_directories(edepsim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edepsim PUBLIC ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# Install the G4 macro files used to control the MC.
install(FILES edepsim-defaults-1.0.mac DESTINATION lib/edep-sim)

# Install the library for edep-sim
install(TARGETS edepsim LIBRARY DESTINATION lib)

# Install the header files.
install(FILES ${includes} DESTINATION include/edep-sim)

# If this is ROOT6 or later, then install the rootmap and pcm files.
if(${ROOT_VERSION} VERSION_GREATER 6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim.rootmap
    DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim_rdict.pcm
    DESTINATION lib)
endif(${ROOT_VERSION} VERSION_GREATER 6)
