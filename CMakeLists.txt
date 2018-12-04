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
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(OPT_MATLAB "Add support for Matlab via MexIFace." ON)
option(OPT_TESTING "Support testing features" OFF)
option(OPT_DEBUG "Support debugging features" OFF)
option(OPT_EXTRA_DEBUG "Support extra noisy debugging features" OFF) #Extra debug features (Armadillo)

message(STATUS "OPTION: BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message(STATUS "OPTION: BUILD_STATIC_LIBS: ${BUILD_STATIC_LIBS}")
message(STATUS "OPTION: OPT_MATLAB: ${OPT_MATLAB}")
message(STATUS "OPTION: OPT_TESTING: ${OPT_TESTING}")
message(STATUS "OPTION: OPT_DEBUG: ${OPT_DEBUG}")
message(STATUS "OPTION: OPT_EXTRA_DEBUG: ${OPT_EXTRA_DEBUG}") 

#Add UcommonCmakeModules git subpreo to path.
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake/UncommonCmakeModules)

# Standard configuration for a numerical computation environment 
#  * Armadillo 
#  * OpenMP
#  * LAPACK
#  * Blas
#  * Pthreads 
include(ConfigureNumericalDependenciesArmadillo)

#Configure standard CFlags and definitions for debug builds
include(ConfigureDebugBuilds) 

#Configure cmake packaging for easy integration into other CMake builds
include(ExportPackageWizzard)
export_package_wizzard()

add_subdirectory(src) #library source

if(OPT_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(src/test)
endif()

if(OPT_MATLAB)
    #MexIFace allows for cross-building of .mex modules for matlab in linux win64 and macosX
    message(STATUS "*** Matlab Module Building Enabled ***")
    set(MexIFaceURL "https://github.com/markjolah/MexIFace.git" CACHE STRING "URL of MexIFace library dependency")
    include(AddExternalDependency)
    add_external_dependency(NAME MexIFace URL ${MexIFaceURL} SHARED)
    include(MexIFace) #Configure MexIFace environment
    message(STATUS "MexIFace initialized")
    add_subdirectory(src/MexIFace) #Build C++ MATLAB Modules
endif()
