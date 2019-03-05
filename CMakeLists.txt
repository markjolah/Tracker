# Tracker - Main CMakeLists.txt
#
# A C++ and Matlab interface for trajectory connections in single particle tracking
# using C++ linear assignment problem (LAP) solvers.
#
# Mark J. Olah (mjo@cs.unm DOT edu)
# Copyright 2014-2019
# Licensed under the Apache License, Version 2.0
# https://www.apache.org/licenses/LICENSE-2.0
# See: LICENSE file

cmake_minimum_required( VERSION 3.9 )
project(Tracker VERSION 0.1 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" OFF)
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    option(BUILD_TESTING "Build testing framework" ON)
else()
    option(BUILD_TESTING "Build testing framework" OFF)
endif()
option(OPT_DOC "Build documentation" OFF)
option(OPT_INSTALL_TESTING "Install testing executables" OFF)
option(OPT_EXPORT_BUILD_TREE "Configure the package so it is usable from the build tree.  Useful for development." OFF)
option(OPT_MATLAB "Build and install matlab mex modules and code" OFF)

if(OPT_MATLAB AND NOT OPT_BLAS_INT64)
    set(OPT_BLAS_INT64 True)
    set(OPT_BLAS_INT64 True CACHE BOOL "Use 64-bit integers for Armadillo, BLAS, and LAPACK. [Forced on by OPT_MATLAB]." FORCE)
endif()

#Set the default CMAKE_BUILD_TYPE to release is unspecified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "CMake build type." FORCE)
    set(CACHE CAMKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "OPTION: BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message(STATUS "OPTION: BUILD_STATIC_LIBS: ${BUILD_STATIC_LIBS}")
message(STATUS "OPTION: BUILD_TESTING: ${BUILD_TESTING}")
message(STATUS "OPTION: OPT_DOC: ${OPT_DOC}")
message(STATUS "OPTION: OPT_INSTALL_TESTING: ${OPT_INSTALL_TESTING}")
message(STATUS "OPTION: OPT_EXPORT_BUILD_TREE: ${OPT_EXPORT_BUILD_TREE}")
message(STATUS "OPTION: OPT_MATLAB: ${OPT_MATLAB}")

#Add UcommonCmakeModules git subpreo to path.
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake/UncommonCMakeModules)

### Dependencies
include(AddExternalDependency)
#BacktraceException allows for exceptions that encode a backtrace for debugging
set(BacktraceExceptionURL https://github.com/markjolah/BacktraceException.git CACHE STRING "URL of BacktraceException library dependency")
add_external_dependency(NAME BacktraceException URL ${BacktraceExceptionURL} VARS OPT_INSTALL_TESTING)
if(OPT_MATLAB)
    set(MexIFaceURL https://github.com/markjolah/MexIFace.git CACHE STRING "URL of MexIFace library dependency")
    add_external_dependency(NAME MexIFace URL ${MexIFaceURL} VARS OPT_INSTALL_TESTING)
endif()

#Armadillo
find_package(OpenMP REQUIRED)
find_package(Armadillo REQUIRED COMPONENTS CXX11)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${ARMADILLO_PRIVATE_COMPILE_DEFINITIONS})

include(ConfigureDebugBuilds) #setup debug build configurations

### PackageConfig Exports from UncommonCMakeModules/ExportPackageWizzard.cmake
#setup build-tree and install-tree exports and PackageConfig files
include(ExportPackageWizzard)
set(EXPORTED_FIND_MODULES cmake/UncommonCMakeModules/FindArmadillo.cmake)
export_package_wizzard(FIND_MODULES ${EXPORTED_FIND_MODULES})

### Main Library
add_subdirectory(src)

### Testing
if(BUILD_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

### Matlab - MexIFace module
if(OPT_MATLAB)
    message(STATUS "*** Matlab Module Building Enabled ***")
    add_subdirectory(src/MexIFace)
    mexiface_configure_install() #Matlab code and startupMappel.m configure and install
endif()

### Documentation
if(OPT_DOC)
    add_subdirectory(doc)
endif()
