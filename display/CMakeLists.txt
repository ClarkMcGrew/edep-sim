cmake_minimum_required(VERSION 3.30)

set(CMAKE_BUILD_TYPE Debug)

# Set the project name and version
project(edep-disp)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("Energy Deposition Display -- ${VERSION}")

# Set the default verbosity of the compilation...
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# Configure the dependencies
find_package(ROOT REQUIRED
  COMPONENTS Geom Physics Matrix MathCore Tree RIO
  Eve TreePlayer Ged RGL Gui Graf Graf3d) 
if(ROOT_FOUND)
  include(${ROOT_USE_FILE})
endif(ROOT_FOUND)

if(ROOT_Eve_LIBRARY)
  message("Building the edep-disp event display")
else(ROOT_Eve_LIBRARY)
  message("NOT BUILDING THE EVENT DISPLAY: eve not found")
  return()
endif(ROOT_Eve_LIBRARY)

# Suck up all of the source files.  This isn't good CMAKE practice,
# but it makes maintaining this file easier.
file(GLOB_RECURSE source *.cxx)

# Compile the base library with private I/O fields.
add_definitions(-DEDEPSIM_FORCE_PRIVATE_FIELDS)

# Make sure the current directories are available for the root
# dictionary generation.
include_directories(${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../io
  ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Build the dictionary for the i/o classes.
ROOT_GENERATE_DICTIONARY(G__edepdisp
  TEventChangeManager.hxx
  OPTIONS -inlineInputHeader
  LINKDEF edepdisp_LinkDef.h)

# Build the library.
add_library(edepdisp SHARED
  ${source}
  G__edepdisp.cxx)
target_include_directories(edepdisp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edepdisp PUBLIC ${ROOT_LIBRARIES} edepsim_io)
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
