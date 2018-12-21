##!/bin/bash

INSTALL_PATH=_install
BUILD_PATH=_build/Debug
NUM_PROCS=`grep -c ^processor /proc/cpuinfo`
COMMON_ARGS="-DCMAKE_INSTALL_PREFIX=$INSTALL_PATH"
rm -rf $INSTALL_PATH
rm -rf $BUILD_PATH

set -e

cmake -H. -B$BUILD_PATH -DCMAKE_BUILD_TYPE=Debug -DOPT_DEBUG=ON -DOPT_TESTING=ON -Wdev ${COMMON_ARGS}
VERBOSE=1 cmake --build $BUILD_PATH --target install -- -j${NUM_PROCS}
