# Copyright (C) 2020, Yeolar

# Define project
project(inja)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-scripts)

# Set project version
set(VERSION_MAJOR 2)
set(VERSION_MINOR 2)
set(VERSION_PATCH x)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
find_package(Accelerator REQUIRED)

# Optional packages
find_package(GTest)

# Setup environment
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
set(CPACK_GENERATOR "DEB;TXZ")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "yeolar@qq.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Template Engine for Modern C++.")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
include(CPack)

