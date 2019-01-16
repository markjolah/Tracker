#
# Tracker - Main CMakeLists.txt
#
# A C++ and Matlab interface for trajectory connections in single particle tracking
# using C++ linear assignment problem (LAP) solvers.
#
# Mark J. Olah (mjo@cs.unm DOT edu)
# Copyright 2014-2018
# Licensed under the Apache License, Version 2.0
# https://www.apache.org/licenses/LICENSE-2.0
# See: LICENCE file

cmake_minimum_required( VERSION 3.9 )
project(Tracker VERSION 0.1 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" OFF)
option(BUILD_TESTING "Build testing code" OFF)
option(OPT_EXPORT_BUILD_TREE "Configure the package so it is usable from the build tree.  Useful for development." OFF)
option(OPT_MATLAB "Build and install matlab mex modules and code" ON)
option(OPT_DEBUG "Enable debugging features" OFF)
option(OPT_EXTRA_DEBUG "Enable extra noisy debugging features" OFF) #Extra debug features (Armadillo)

message(STATUS "OPTION: BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message(STATUS "OPTION: BUILD_STATIC_LIBS: ${BUILD_STATIC_LIBS}")
message(STATUS "OPTION: BUILD_TESTING: ${BUILD_TESTING}")
message(STATUS "OPTION: OPT_EXPORT_BUILD_TREE: ${OPT_EXPORT_BUILD_TREE}")
message(STATUS "OPTION: OPT_MATLAB: ${OPT_MATLAB}")
message(STATUS "OPTION: OPT_DEBUG: ${OPT_DEBUG}")
message(STATUS "OPTION: OPT_EXTRA_DEBUG: ${OPT_EXTRA_DEBUG}") 

#Add UcommonCmakeModules git subpreo to path.
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake/UncommonCmakeModules)

### Dependencies
#Armadillo
find_package(Armadillo REQUIRED)
add_definitions(-DARMA_USE_CXX11)
add_definitions(-DARMA_DONT_USE_LAPACK -DARMA_DONT_USE_BLAS -DARMA_DONT_USE_OPENMP -DARMA_DONT_USE_HDF5) #disable unused dependencies
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:ARMA_PRINT_ERRORS>)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<NOT:$<CONFIG:Debug>>:ARMA_NO_DEBUG>)
if(OPT_EXTRA_DEBUG)
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:ARMA_EXTRA_DEBUG>)
endif()
add_compile_options($<$<CONFIG:Debug>:-Wno-unused-local-typedefs>) # Necessary for armadillo 9.200.6 warnings

#OpenMP
find_package(OpenMP REQUIRED)

include(AddExternalDependency)
#BacktraceException allows for exceptions that encode a backtrace for debugging
add_external_dependency(NAME BacktraceException URL https://github.com/markjolah/BacktraceException.git SHARED)

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
    #MexIFace allows for cross-building of .mex modules for matlab in linux win64 and macosX
    message(STATUS "*** Matlab Module Building Enabled ***")
    add_external_dependency(NAME MexIFace URL "https://github.com/markjolah/MexIFace.git" SHARED)
    add_subdirectory(src/MexIFace) #Build C++ MATLAB Modules
    mexiface_configure_install() #Matlab code and startupTracker.m configure and install
endif()
