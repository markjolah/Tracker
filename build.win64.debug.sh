#!/bin/bash
#
# build.win64.debug.sh
#
#
ARCH=win64
FULL_ARCH=x86_64-w64-mingw32
TOOLCHAIN_FILE=./cmake/UncommonCMakeModules/Toolchains/Toolchain-MXE-${FULL_ARCH}.cmake
INSTALL_PATH=_${ARCH}.install
BUILD_PATH=_${ARCH}.build/Debug
NUM_PROCS=`grep -c ^processor /proc/cpuinfo`
ARGS="-DBUILD_TESTING=1"
ARGS="${ARGS} -DOPT_MATLAB=On"
ARGS="${ARGS} -DOPT_FIXUP_DEPENDENCIES=On"
ARGS="${ARGS} -DOPT_EXPORT_BUILD_TREE=On"
ARGS="${ARGS} -DOPT_FIXUP_BUILD_TREE_DEPENDENCIES=On"
rm -rf $INSTALL_PATH $BUILD_PATH

set -ex

cmake -H. -B$BUILD_PATH -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH -DCMAKE_BUILD_TYPE=Debug ${ARGS}
VERBOSE=1 cmake --build $BUILD_PATH --target install -- -j${NUM_PROCS}
