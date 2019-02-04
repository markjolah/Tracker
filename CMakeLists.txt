#
# Tracker - Main CMakeLists.txt
#
# A C++ and Matlab interface for trajectory connections in single particle tracking
# using C++ linear assignment problem (LAP) solvers.
#
# Mark J. Olah (mjo@cs.unm DOT edu)
# Copyright 2014-2019
# Licensed under the Apache License, Version 2.0
# https://www.apache.org/licenses/LICENSE-2.0
# See: LICENCE file

cmake_minimum_required( VERSION 3.9 )
project(Tracker VERSION 0.1 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" OFF)
f(${CMAKE_BUILD_TYPE} MATCHES Debug)
    option(BUILD_TESTING "Build testing framework" ON)
else()
    option(BUILD_TESTING "Build testing framework" OFF)
endif()
option(OPT_INSTALL_TESTING "Install testing executables" OFF)
option(OPT_EXPORT_BUILD_TREE "Configure the package so it is usable from the build tree.  Useful for development." OFF)
option(OPT_ARMADILLO_INT64 "Use 64-bit integers for Armadillo, BLAS, and LAPACK. [Must be on if OPT_MATLAB is on]" ON)
option(OPT_MATLAB "Build and install matlab mex modules and code" ON)

if(OPT_MATLAB AND NOT OPT_ARMADILLO_INT64)
    set(OPT_ARMADILLO_INT64 True)
    set(OPT_ARMADILLO_INT64 True CACHE BOOL "Use 64-bit integers for Armadillo, BLAS, and LAPACK. [Forced on by OPT_MATLAB]." FORCE)
endif()

message(STATUS "OPTION: BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message(STATUS "OPTION: BUILD_STATIC_LIBS: ${BUILD_STATIC_LIBS}")
message(STATUS "OPTION: BUILD_TESTING: ${BUILD_TESTING}")
message(STATUS "OPTION: OPT_INSTALL_TESTING: ${OPT_INSTALL_TESTING}")
message(STATUS "OPTION: OPT_EXPORT_BUILD_TREE: ${OPT_EXPORT_BUILD_TREE}")
message(STATUS "OPTION: OPT_ARMADILLO_INT64: ${OPT_ARMADILLO_INT64}")
message(STATUS "OPTION: OPT_MATLAB: ${OPT_MATLAB}")

#Add UcommonCmakeModules git subpreo to path.
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake/UncommonCMakeModules)

### Dependencies
#Armadillo
if(OPT_ARMADILLO_INT64)
    set(ARMADILLO_INT64_COMPONENT INT64) #Enable INT64 interface for armadillo, Blas and Lapack
else()
    set(ARMADILLO_INT64_COMPONENT)
endif()
find_package(Armadillo REQUIRED COMPONENTS CXX11 ${ARMADILLO_INT64_COMPONENT})
# add_compile_options($<$<CONFIG:Debug>:-Wno-unused-local-typedefs>) # Necessary for armadillo 9.200.6 warnings

#OpenMP
find_package(OpenMP REQUIRED)

include(AddExternalDependency)
#BacktraceException allows for exceptions that encode a backtrace for debugging
add_external_dependency(NAME BacktraceException URL https://github.com/markjolah/BacktraceException.git SHARED TESTING)

#setup debug build configurations
include(ConfigureDebugBuilds)

### PackageConfig Exports
include(ExportPackageWizzard)
export_package_wizzard(EXPORT_BUILD_TREE "${OPT_EXPORT_BUILD_TREE}") #setup build-tree and install-tree exports and packageconfig files


add_subdirectory(src) #library source

if(BUILD_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(src/test)
endif()

if(OPT_MATLAB)
    message(STATUS "*** Matlab Module Building Enabled ***")
    set(MexIFaceURL https://github.com/markjolah/MexIFace.git CACHE STRING "URL of MexIFace library dependency")
    add_external_dependency(NAME MexIFace URL ${MexIFaceURL} SHARED TESTING)
    add_subdirectory(src/MexIFace)
    mexiface_configure_install(EXPORT_BUILD_TREE "${OPT_EXPORT_BUILD_TREE}") #Matlab code and startupMappel.m configure and install
endif()
