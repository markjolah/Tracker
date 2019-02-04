##!/bin/bash
# build.debug.sh <cmake args ...>
# Debug-only build to local install prefix with build-tree export


INSTALL_PATH=_install
BUILD_PATH=_build/Debug
NUM_PROCS=`grep -c ^processor /proc/cpuinfo`
ARGS=""
ARGS="${ARGS} -DBUILD_TESTING=On"
ARGS="${ARGS} -DOPT_MATLAB=Off"
ARGS="${ARGS} -DOPT_DEBUG=On"
ARGS="${ARGS} -DOPT_EXPORT_BUILD_TREE=On"

set -ex
rm -rf $BUILD_PATH

cmake -H. -B$BUILD_PATH -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH -DCMAKE_BUILD_TYPE=Debug -Wdev ${ARGS} $@
VERBOSE=1 cmake --build $BUILD_PATH --target install -- -j${NUM_PROCS}
