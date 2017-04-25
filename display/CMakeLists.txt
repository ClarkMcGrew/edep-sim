# Suck up all of the .cc files for the source.  This isn't good CMAKE
# practice, but it makes maintaining this file easier.
file(GLOB_RECURSE source *.cxx)

# Make sure the current directories are available for the root
# dictionary generation.
include_directories(${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/../src)


# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__edepdisp
  TEventChangeManager.hxx
  LINKDEF edepdisp_LinkDef.h)

# Build the library.
add_library(edepdisp SHARED ${source} G__edepdisp.cxx)
target_include_directories(edepdisp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edepdisp PUBLIC ${ROOT_EVE_LIBRARY} ${ROOT_LIBRARIES})
install(TARGETS edepdisp LIBRARY DESTINATION lib)

message(${ROOT_LIBRARIES})

# Build the display
add_executable(edep-disp eventDisplay.cxx)
target_link_libraries(edep-disp LINK_PUBLIC edepdisp)
target_link_libraries(edep-disp LINK_PUBLIC edepsim)
install(TARGETS edep-disp RUNTIME DESTINATION bin)

# If this is ROOT6 or later, then install the rootmap and pcm files.
if(${ROOT_VERSION} VERSION_GREATER 6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepdisp.rootmap
    DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepdisp_rdict.pcm
    DESTINATION lib)
endif(${ROOT_VERSION} VERSION_GREATER 6)
