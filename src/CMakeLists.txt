file(GLOB_RECURSE source *.cc)

ROOT_GENERATE_DICTIONARY(G__edepsim
  TG4PrimaryVertex.hxx TG4Trajectory.hxx TG4HitSegment.hxx TG4Event.hxx
  LINKDEF edepsim_LinkDef.h)
  
include_directories(${CMAKE_CURRENT_LIST_DIR})
add_library(edepsim SHARED ${source}
  TG4PrimaryVertex.cxx  TG4Trajectory.cxx TG4HitSegment.cxx TG4Event.cxx
  G__edepsim.cxx)
target_include_directories(edepsim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edepsim PUBLIC ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

install(FILES edepsim-defaults-1.0.mac DESTINATION lib/edep-sim)

install(TARGETS edepsim LIBRARY DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim.rootmap DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepsim_rdict.pcm DESTINATION lib)
