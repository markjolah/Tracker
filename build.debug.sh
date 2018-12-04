##!/bin/bash

INSTALL_PATH=_install
BUILD_PATH=_build
NUM_PROCS=`grep -c ^processor /proc/cpuinfo`
COMMON_ARGS="-DCMAKE_INSTALL_PREFIX=$INSTALL_PATH"
rm -rf $INSTALL_PATH/Debug

set -e

cmake -H. -B$BUILD_PATH/Debug -DCMAKE_BUILD_TYPE=Debug -DOPT_TESTING=ON ${COMMON_ARGS}
VERBOSE=1 cmake --build $BUILD_PATH/Debug --target install -- -j${NUM_PROCS}
