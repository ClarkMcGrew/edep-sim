message("Energy Deposition Simulation")

# Suck up all of the .cc files for the source.  This isn't good CMAKE
# practice, but it makes maintaining this file easier.
file(GLOB_RECURSE source *.cc)
file(GLOB_RECURSE includes EDepSim*.hh)

# Where the macro files can be found.
add_definitions(-DDETSIM_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

# Compile the base library with private I/O fields.
add_definitions(-DEDEPSIM_FORCE_PRIVATE_FIELDS)

# Build the library.
add_library(edepsim SHARED ${source})

target_include_directories(edepsim PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}../io>"
  "$<INSTALL_INTERFACE:include/EDepSim>")

target_link_libraries(edepsim PUBLIC
  edepsim_io ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# Install the G4 macro files used to control the MC.
install(FILES edepsim-defaults-1.0.mac DESTINATION lib/EDepSim)

# Install the library for edep-sim
install(TARGETS edepsim
  EXPORT EDepSimTargets
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/EDepSim )

# Install the header files.
install(FILES ${includes} DESTINATION include/EDepSim)


