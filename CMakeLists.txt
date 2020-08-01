# Copyright (C) 2020, Yeolar

cmake_minimum_required(VERSION 3.2)

# Define project
project(inja VERSION 2.2.0)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-scripts)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(Boost_USE_MULTITHREADED TRUE)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS random regex thread)
find_package(Accelerator REQUIRED)

# Optional packages
find_package(GTest)

# Setup environment
option(BUILD_SHARED_LIBS "Build libraries as shared ones" OFF)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")    # memcheck
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Link libraries
link_libraries(
  ${ACCELERATOR_LIBRARY}
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_PREFIX_PATH}/include
)

# Check and setup configurations
configure_file(
  ${PROJECT_SOURCE_DIR}/inja/inja-config.h.in
  ${PROJECT_SOURCE_DIR}/inja/inja-config.h
)

# Work on all sub directories
add_subdirectory(inja)

# Binary

# Test
if(GTEST_FOUND)
  enable_testing()
  add_subdirectory(inja/test)
endif()

# CmakeConfig
include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_MODULE_PATH}/inja-config.cmake.in
  inja-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/inja
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/inja-config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/inja
)

# Pack
set(CPACK_SYSTEM_NAME "amd64")
find_program(DPKG_CMD dpkg)
if(DPKG_CMD)
  execute_process(
    COMMAND "${DPKG_CMD}" --print-architecture
    OUTPUT_VARIABLE CPACK_SYSTEM_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

include(InstallRequiredSystemLibraries)
if(DPKG_CMD)
  set(CPACK_GENERATOR "DEB;TXZ")
else()
  set(CPACK_GENERATOR "TXZ")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "yeolar@qq.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Template Engine for Modern C++.")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
include(CPack)

