cmake_minimum_required(VERSION 3.12)

project(libgkdi)

execute_process(
    COMMAND
    bash -c "grep 'Version:' .gear/libgkdi.spec | awk -F ':' '{ print $2 }' | tr -d [:space:]"
    OUTPUT_VARIABLE LIB_GKDI_GEAR_VERSION
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

string(REPLACE "." ";" GEAR_VERSION_LIST ${LIB_GKDI_GEAR_VERSION})
list(GET GEAR_VERSION_LIST 0 LIB_GKDI_VERSION_MAJOR)
list(GET GEAR_VERSION_LIST 1 LIB_GKDI_VERSION_MINOR)
list(GET GEAR_VERSION_LIST 2 LIB_GKDI_VERSION_PATCH)

if(UNIX)
  include(GNUInstallDirs)
else()
    message(FATAL_ERROR "System not UNIX - not implemented yet")
endif()

set(GKDI_IN_BUILD_TREE TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

configure_file(
    "${CMAKE_MODULE_PATH}/GkdiConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/GkdiConfig.cmake"
    IMMEDIATE @ONLY
)
install(FILES ${CMAKE_BINARY_DIR}/GkdiConfig.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/gkdi")

configure_file(
    "${CMAKE_MODULE_PATH}/GkdiBuildTreeSettings.cmake.in"
    "${CMAKE_BINARY_DIR}/GkdiBuildTreeSettings.cmake"
    IMMEDIATE @ONLY
)

export(PACKAGE Gkdi)

include(FindIDL)

add_subdirectory(examples)

add_subdirectory(src)

option(GKDI_BUILD_TESTS "Build gkdi tests." ON)
option(GKDI_BUILD_EXAMPLES "Build gkdi examples." ON)

enable_testing()
add_subdirectory(tests)
