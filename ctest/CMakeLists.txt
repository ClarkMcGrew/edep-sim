# ctest/CMakeLists.txt
#
# CTest tests for the EDepSim::TrackingService / TrackingWorker / TrackingChannel
# facility (the thread-affine Geant4 lifecycle).  Enabled by include(CTest) in
# the top-level CMakeLists; this directory is only added when Geant4 is present
# (see the NOT EDEPSIM_READONLY block).
#
# The test executables link the `edepsim` target, which transitively provides
# the src/ + io/ include directories and the Geant4/ROOT libraries.  Tests that
# CONSTRUCT TG4 primaries are compiled with EDEPSIM_USE_PUBLIC_FIELDS because
# TG4PrimaryParticle's i/o fields are otherwise private (see the note in
# EDepSimTrackingTestPrimaries.hh).
#
# The Geant4-driven tests need the Geant4 dataset environment (G4*DATA) at run
# time, exactly like the scripts under validate/; run ctest in the same
# environment used to run edep-sim.

set(_geometry ${CMAKE_CURRENT_SOURCE_DIR}/../inputs/example.gdml)

# --- channel protocol: no Geant4, no geometry ------------------------------
add_executable(edepTrackingChannelTest EDepSimTrackingChannelTest.cc)
target_link_libraries(edepTrackingChannelTest PRIVATE edepsim_service)
add_test(NAME TrackingChannel COMMAND edepTrackingChannelTest)

# --- exception translation across the worker thread: no Geant4 init --------
add_executable(edepTrackingServiceErrorTest EDepSimTrackingServiceErrorTest.cc)
target_link_libraries(edepTrackingServiceErrorTest PRIVATE edepsim_service)
add_test(NAME TrackingServiceError COMMAND edepTrackingServiceErrorTest)

# --- full one-event integration (threaded service) -------------------------
add_executable(edepTrackingServiceSimTest EDepSimTrackingServiceSimTest.cc)
target_compile_definitions(edepTrackingServiceSimTest PRIVATE EDEPSIM_USE_PUBLIC_FIELDS)
target_link_libraries(edepTrackingServiceSimTest PRIVATE edepsim_service)
add_test(NAME TrackingServiceSim
  COMMAND edepTrackingServiceSimTest ${_geometry})

# --- adopt mode: Geant4 on the calling thread ------------------------------
add_executable(edepTrackingServiceAdoptTest EDepSimTrackingServiceAdoptTest.cc)
target_compile_definitions(edepTrackingServiceAdoptTest PRIVATE EDEPSIM_USE_PUBLIC_FIELDS)
target_link_libraries(edepTrackingServiceAdoptTest PRIVATE edepsim_service)
add_test(NAME TrackingServiceAdopt
  COMMAND edepTrackingServiceAdoptTest ${_geometry})

# --- custom generator injected via a factory (built on the G4 thread) ------
# Builds G4 primaries directly, so no EDEPSIM_USE_PUBLIC_FIELDS is needed.
add_executable(edepTrackingServiceGeneratorTest EDepSimTrackingServiceGeneratorTest.cc)
target_link_libraries(edepTrackingServiceGeneratorTest PRIVATE edepsim_service)
add_test(NAME TrackingServiceGenerator
  COMMAND edepTrackingServiceGeneratorTest ${_geometry})
