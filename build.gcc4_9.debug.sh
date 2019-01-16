#!/bin/bash
#
#gcc4_9.build.sh
#
#


ARCH=gcc4_9
FULL_ARCH=x86_64-${ARCH}-linux-gnu
TOOLCHAIN_FILE=./cmake/Toolchains/Toolchain-${FULL_ARCH}.cmake
INSTALL_PATH=_${ARCH}.install
BUILD_PATH=_${ARCH}.build/Debug
NUM_PROCS=`grep -c ^processor /proc/cpuinfo`
ARGS="-DBUILD_TESTING=1 -DOPT_INSTALL_GCC_DEPENDENCIES=1"
rm -rf $INSTALL_PATH $BUILD_PATH

set -e

cmake -H. -B$BUILD_PATH -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" -DCMAKE_BUILD_TYPE=Debug ${ARGS}
VERBOSE=1 cmake --build $BUILD_PATH --target install -- -j${NUM_PROCS}
