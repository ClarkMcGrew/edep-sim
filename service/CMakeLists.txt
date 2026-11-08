message(STATUS "Building EDepSim tracking service")

# The thread-affine Geant4 lifecycle facility (see doc/TrackingService.md).
# It layers on top of the core edep-sim library.

set(source
  EDepSimTrackingChannel.cc
  EDepSimPrimaryVertexGenerator.cc
  EDepSimTrackingWorker.cc
  EDepSimTrackingService.cc)

set(includes
  EDepSimTrackingMessages.hh
  EDepSimTrackingChannel.hh
  EDepSimPrimaryVertexGenerator.hh
  EDepSimTrackingWorker.hh
  EDepSimTrackingService.hh)

add_library(edepsim_service SHARED ${source})

target_include_directories(edepsim_service PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:include/EDepSim>")

# Linking the core library transitively provides edepsim_io, Geant4 and ROOT,
# and the src/ + io/ include directories the service headers/sources use.
target_link_libraries(edepsim_service PUBLIC edepsim)

# Install the library for edep-sim (same export set as the other targets).
install(TARGETS edepsim_service
  EXPORT EDepSimTargets
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/EDepSim)

# Install the header files.
install(FILES ${includes} DESTINATION include/EDepSim)
