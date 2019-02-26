<a href="https://travis-ci.org/markjolah/Tracker"><img src="https://travis-ci.org/markjolah/Tracker.svg?branch=master"/></a>
# Tracker
Tracker is a particle tracking trajectory connector tool that is based on a sparse-matrix [linear assignment problem (LAP)](https://en.wikipedia.org/wiki/Assignment_problem) solver.
 * Tracker uses a sparse matrix formulations of the [Jonker-Volgenant Algorithm](https://dl.acm.org/citation.cfm?id=30107)
 * Tracker provides C++ and Matlab object-oriented interfaces. [`tracker::LAPTrack`](http://markolah.pecos.us/Tracker/classtracker_1_1LAPTrack.html)
 * Tracker is designed for cross-platform complilation to Linux and Windows 64-bit targets.

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

## Dependencies

* [*Armadillo*](http://arma.sourceforge.net/docs.html) - A high-performance array library for C++.

### External Projects
These packages are specialized CMake projects.  If they are not currently installed, at the start of the build process, the [AddExternalDependency.cmake](https://github.com/markjolah/UncommonCMakeModules/blob/master/AddExternalDependency.cmake) will automatically download, configure, build and install CMake-based projects to the `CMAKE_INSTALL_PREFIX`.  This process is completed before CMake configure-time so calls to the normal `find_package()` command are used to find the auto-added Dependencies.

- [BacktraceException](https://markjolah.github.io/BacktraceException) - A library to provide debugging output on exception calls.  Important for Matlab debugging.
- [MexIFace](https://markjolah.github.io/MexIFace) - A C++/Matlab object oriented interface library for high-performance numerical computations.  Provides cross-compilation to Matlab R2016b+ target environments on Linux and Windows 64-bit targets.



