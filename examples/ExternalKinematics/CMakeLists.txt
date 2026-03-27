cmake_minimum_required(VERSION 3.30)

# Set the project name and version.  The project name will be used to
# import this into other CMAKE files.
project(EDepSimKinemExample VERSION 0.0.0)

message(STATUS "EDepSim Dynamic Load Kinematics Example")

# Define the options that can be set in the cache, or on the cmake
# command line.
set(CMAKE_BUILD_TYPE Debug)

# Find EDepSim
find_package(EDepSim REQUIRED)

# Make sure that ROOT and EDepSim agree on the CXX standard
set(IS_ROOT_CXX_STANDARD_SET OFF)
get_target_property(root_features ROOT::Core INTERFACE_COMPILE_FEATURES)
foreach(cxxstd IN ITEMS 23 20 17 14 11)
  if("cxx_std_${cxxstd}" IN_LIST root_features)
    message(STATUS "ROOT compiled with C++${cxxstd}")
    if (NOT cxxstd EQUAL CMAKE_CXX_STANDARD)
      message(STATUS "EDepSim CXX Standard is " ${CMAKE_CXX_STANDARD})
      message(FATAL "ROOT CXX Standard does not match EDepSim")
    endif()
    break()
  endif()
endforeach()
if (!IS_ROOT_CXX_STANDARD_SET)
  message(FATAL "Could not find ROOT CXX standard")
endif()

file(GLOB kinem_source src/*.cc)
add_library(EDepExampleKinem SHARED ${kinem_source})
target_link_libraries(EDepExampleKinem PUBLIC EDepSim::edepsim)
