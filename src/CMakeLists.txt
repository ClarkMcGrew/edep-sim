message("Energy Deposition Simulation")

# Suck up all of the .cc files for the source.  This isn't good CMAKE
# practice, but it makes maintaining this file easier.
# file(GLOB_RECURSE source *.cc)
# file(GLOB_RECURSE includes EDepSim*.hh)
file(GLOB edep_source *.cc)
file(GLOB edep_includes *.cc)
file(GLOB captain_source captain/*.cc)
file(GLOB captain_includes captain/EDepSim*.hh)
file(GLOB kinem_source kinem/*.cc)
file(GLOB kinem_includes kinem/EDepSim*.hh)

if(EDEPSIM_USE_PARTICLEBOMB)
  file(GLOB bomb_source kinem/ParticleBomb/*.cc)
  file(GLOB bomb_includes kinem/ParticleBomb/*.hh)
  add_definitions(-DEDEPSIM_USE_PARTICLEBOMB)
else(EDEPSIM_USE_PARTICLEBOMB)
  message("Not including ParticleBomb (MPV/MPR) in the compilation")
endif(EDEPSIM_USE_PARTICLEBOMB)

if(EDEPSIM_USE_NEST)
  file(GLOB nest_source NESTVersion098/*.cc)
  file(GLOB nest_includes NESTVersion098/EDepSim*.hh)
else(EDEPSIM_USE_NEST)
  message("Not including NEST in the compilation")
  add_definitions(-DEDEPSIM_SKIP_NESTVersion098)
endif(EDEPSIM_USE_NEST)

set(source ${edep_source} ${captain_source} ${kinem_source} ${nest_source} ${bomb_source})
set(includes ${edep_includes} ${captain_includes} ${kinem_includes} ${bomb_includes})

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

if(EDEPSIM_USE_PARTICLEBOMB)
  find_package( yaml-cpp CONFIG REQUIRED )
  target_include_directories(edepsim PRIVATE
    ${YAML_CPP_INCLUDE_DIR} $ENV{DLPGENERATOR_INCDIR}
  )
  target_link_directories(edepsim PUBLIC $ENV{DLPGENERATOR_LIBDIR})
  target_link_libraries(edepsim PUBLIC yaml-cpp LiteFMWK_ParticleBomb)
endif(EDEPSIM_USE_PARTICLEBOMB)

target_link_libraries(edepsim PUBLIC
  edepsim_io ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} 
)

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
