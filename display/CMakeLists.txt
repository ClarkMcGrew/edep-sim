# Suck up all of the source files.  This isn't good CMAKE practice,
# but it makes maintaining this file easier.
file(GLOB_RECURSE source *.cxx)

# Suck up the output definition files from the edepsim library.  Don't
# ever do this!!!!  The I/O should really be broken into a separate
# library that can be exported.
file(GLOB stolenSource ../src/TG4*.cxx)
file(GLOB stolenIncludes ../src/TG4*.h)

# Make sure the current directories are available for the root
# dictionary generation.
include_directories(${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/../src)

message("The stolen source is ${stolenSource}")

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__edepdisp
  TEventChangeManager.hxx
  LINKDEF edepdisp_LinkDef.h)

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__edepdisp_edepsim
  ${stolenIncludes}
  LINKDEF ../src/edepsim_LinkDef.h)

# Build the library.
add_library(edepdisp SHARED
  ${source} G__edepdisp.cxx
  ${stolenSource} G__edepdisp_edepsim.cxx)
target_include_directories(edepdisp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edepdisp PUBLIC ${ROOT_LIBRARIES})
install(TARGETS edepdisp LIBRARY DESTINATION lib)

# Build the display
add_executable(edep-disp eventDisplay.cxx)
target_link_libraries(edep-disp LINK_PUBLIC edepdisp)
install(TARGETS edep-disp RUNTIME DESTINATION bin)

# If this is ROOT6 or later, then install the rootmap and pcm files.
if(${ROOT_VERSION} VERSION_GREATER 6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepdisp.rootmap
    DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libedepdisp_rdict.pcm
    DESTINATION lib)
endif(${ROOT_VERSION} VERSION_GREATER 6)
