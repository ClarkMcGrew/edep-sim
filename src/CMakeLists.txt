# Suck up all of the .cc files for the source.  This isn't good CMAKE
# practice, but it makes maintaining this file easier.
file(GLOB_RECURSE source *.cc)
file(GLOB_RECURSE includes EDepSim*.hh)

# Make sure the current directories are available for the root
# dictionary generation.
include_directories(${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Where the macro files can be found.
add_definitions(-DDETSIM_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

# Compile the base library with private I/O fields.
add_definitions(-DEDEPSIM_FORCE_PRIVATE_FIELDS)

# Build the library.
add_library(edepsim SHARED ${source})
target_include_directories(edepsim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edepsim PUBLIC edepsim_io ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# Install the G4 macro files used to control the MC.
install(FILES edepsim-defaults-1.0.mac DESTINATION lib/edep-sim)

# Install the library for edep-sim
install(TARGETS edepsim LIBRARY DESTINATION lib)

# Install the header files.
install(FILES ${includes} DESTINATION include/edep-sim)

