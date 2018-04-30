cmake_minimum_required (VERSION 3.9.0)
project (NetChannelTranition)


set(SRC_DEPENDENCIS_VERSION 2)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/Source/)

include(${SOURCE_DIR}/CMake/Properties.cmake)
include(${SOURCE_DIR}/CMake/FindPackageOrBuild.cmake)
include(${SOURCE_DIR}/CMake/HelperMethods.cmake)

check_and_update_binary_deps(NCT ${PROJECT_SOURCE_DIR}/Dependencies/ ${SRC_DEPENDENCIS_VERSION})

add_subdirectory(${SOURCE_DIR})
