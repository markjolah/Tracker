<a href="https://travis-ci.org/markjolah/Tracker"><img src="https://travis-ci.org/markjolah/Tracker.svg?branch=master"/></a>
# Tracker
Tracker is a particle tracking trajectory connector tool that generates trajectories by tracking swarms of interacting particles through a sequence of video frames.
Tracker formulates optimal trajectory connection problems as instances of the  [linear assignment problem (LAP)](https://en.wikipedia.org/wiki/Assignment_problem), and
uses the a sparse-matrix implementation of the [Jonker-Volgenant Algorithm](https://dl.acm.org/citation.cfm?id=30107) to solve the LAP problems.
 * Tracker provides C++ and Matlab object-oriented interfaces. [`tracker::LAPTrack`](http://markolah.pecos.us/Tracker/classtracker_1_1LAPTrack.html)
 * Tracker is designed for cross-platform compilation to Linux and Windows 64-bit targets.

## Trajectory connection problem

In single particle tracking applications, a set of likely particles are localized for each frame of a video capture.  The goal of trajectory connection is to partition the localizations from all the frames into a set of trajectories.  Each trajectory is a sequence of localizations which are likely to be from the same object (point emitter).

The Tracker library implements a two-phase strategy to trajectory connection.  First a frame-to-frame algorithm sequentially builds a set of trajectories connecting localizations in adjacent frames, next a gap-closing phase connects shorter trajectories across several frames as particles are often not localized in every frame for various reasons including experimental and photo-chemical effects.

<p align="center"><a href="https://raw.githubusercontent.com/markjolah/Tracker/master/doc/images/tracker_problem.png" title="full size image">
<img alt="Figure 1: The frame-to-frame trajectory connection problem" src="https://raw.githubusercontent.com/markjolah/Tracker/master/doc/images/tracker_problem.png" width="750"/></a>
<p align="center">
<strong>Figure 1</strong>: The frame-to-frame trajectory connection problem
</p></p>

## Documentation
The Tracker Doxygen documentation can be build with the `OPT_DOC` CMake option and is also available on online:
  * [Tracker HTML Manual](https://markjolah.github.io/Tracker/index.html)
  * [Tracker PDF Manual](https://markjolah.github.io/Tracker/pdf/Tracker-0.1-reference.pdf)
  * [Tracker github repository](https://github.com/markjolah/Tracker)

## Installing
Tracker uses the CMake build system.  The script [`build.sh`](https://github.com/markjolah/Tracker/blob/master/build.sh) sets the project-specific CMake options
to sensible values and builds the project under `./_build/Release` and installs it to the `./_install` prefix, which can be set with the `INSTALL_PREFIX` environment variable.

    INSTALL_PREFIX="..." ./build.sh <additional cmake args...>

Edit `build.sh` to customize  or alternatively use the CMake gui directly:

    cmake -B $BUILD_DIR -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake-gui $BUILD_DIR
    cmake --build $BUILD_DIR --target install


### Dependencies

* [*Armadillo*](http://arma.sourceforge.net/docs.html) - A high-performance array library for C++.
    * [Ubuntu: [`libarmadillo-dev`](https://packages.ubuntu.com/search?keywords=libarmadillo-dev)]&nbsp;  [Gentoo: [`sci-libs/armadillo`](https://packages.gentoo.org/packages/sci-libs/armadillo)]

#### External Projects
These packages are specialized CMake projects.  If they are not installed on the development system, the [add_external_dependency()](https://github.com/markjolah/UncommonCMakeModules/blob/master/AddExternalDependency.cmake) function will automatically download, configure, build and install them to `CMAKE_INSTALL_PREFIX`.

- [BacktraceException](https://markjolah.github.io/BacktraceException) - A library to provide debugging output on exception calls.  Important for Matlab debugging.
- [MexIFace](https://markjolah.github.io/MexIFace) - MexIFace provides an object-oriented C++/Matlab interface and provides cross-compilation support to build for Matlab target environments on Linux and Windows 64-bit targets.

### CMake options

The following CMake options control the build.
 * `BUILD_SHARED_LIBS` - Build shared libraries
 * `BUILD_STATIC_LIBS` - Build static libraries
 * `BUILD_TESTING` - Build testing framework
 * `OPT_DOC` - Build documentation
 * `OPT_INSTALL_TESTING` - Install testing executables in install-tree.
 * `OPT_EXPORT_BUILD_TREE` - Export the package from the build-tree and place in the [CMake user package registry](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#user-package-registry).
 * `OPT_MATLAB` - Enable matlab module building with MexIFace.

### Building for matlab

See:

## Using tracker

### Using Tracker in C++ applications

Tracker exports a [CMake config-file](https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages), allowing it to be found easily with CMake
build systtems
~~~.cmake
    find_package(Tracker)
    target_link_libraries(${MY_TARGET} Tracker::Tracker)
~~~
In the C++ source
~~~.cxx
#include <Tracker/LAPTrack.h>
tracker::LAPTrack tracker(params);
~~~
### Using Tracker in Matlab applications



## LICENSE

* Copyright: 2013-2019
* Author: Mark J. Olah
* Email: (mjo@cs.unm DOT edu)
* LICENSE: Apache 2.0.  See [LICENSE](https://github.com/markjolah/Tracker/blob/master/LICENSE) file.
