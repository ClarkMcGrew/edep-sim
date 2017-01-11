cmake_minimum_required(VERSION 3.0)

# Set the project name and version
project(edep-sim)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("Energy Deposition Simulation -- ${VERSION}")

# Set the default verbosity of the compilation...
set(CMAKE_VERBOSE_MAKEFILE TRUE)

# Configure the dependencies
find_package(ROOT REQUIRED COMPONENTS Geom)
include(${ROOT_USE_FILE})

# Configure GEANT4
find_package(Geant4)
include(${Geant4_USE_FILE})

add_definitions(-DDETSIM_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

# Build the subdirectories
add_subdirectory(src)
add_subdirectory(app)

