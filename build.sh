#!/bin/bash
# build.sh <cmake-args...>
#
# Tracker default build script.
#
# Clean release build to local install prefix.
# * CMAKE_BUILD_TYPE=Release
# * Builds to _build/Release
# * Installs to _install unless INSTALL_PREFIX environment variable is set
# * Deletes build directory
# * Deletes install directory if and only if it is the default value of "_install"
#
# Edit variables as needed.
if [ -n "$INSTALL_PREFIX" ]; then
    INSTALL_PATH=$INSTALL_PREFIX
else
    INSTALL_PATH=_install
fi
BUILD_PATH=_build/Release
NUM_PROCS=`grep -c ^processor /proc/cpuinfo`

ARGS="-DCMAKE_INSTALL_PREFIX=$INSTALL_PATH"
ARGS="${ARGS} -DBUILD_STATIC_LIBS=On"
ARGS="${ARGS} -DBUILD_SHARED_LIBS=On"
ARGS="${ARGS} -DOPT_DOC=Off"
ARGS="${ARGS} -DBUILD_TESTING=On"
ARGS="${ARGS} -DOPT_INSTALL_TESTING=On"
ARGS="${ARGS} -DOPT_EXPORT_BUILD_TREE=Off"
ARGS="${ARGS} -DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=On" #Otherwise dependencies found in build directories won't be found in install tree unless LD_LIBRARY_PATH is modified
ARGS="${ARGS} -DOPT_MATLAB=Off"  #Can only be enabled if older gcc is available

set -ex

if [ "$INSTALL_PATH" == "_install" ]; then
    rm -rf _install
fi

rm -rf $BUILD_PATH
cmake -H. -B$BUILD_PATH -DCMAKE_BUILD_TYPE=Release ${ARGS}
cmake --build $BUILD_PATH --target install -- -j${NUM_PROCS}
