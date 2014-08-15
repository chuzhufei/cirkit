# CirKit

## Requirements

The following software is required in order to build CirKit

* git
* cmake (at least version 2.8.9)
* g++ or clang++ (with C++11 support)
* boost (at least version 1.48.0)
* GNU MP, and its C++ interface GMP++
* Qt5 (only for the GUI)

## Build CirKit

After cloning CirKit perform the following steps

    mkdir build
    cd build
    cmake ..
    make

To build the GUI perform the following steps from the base directory

    cd gui
    mkdir build
    cd build
    cmake ..
    make

## Build CirKit Addons

The easiest way to enable addons is by typing `ccmake ..' in the build directory

## Executing CirKit Programs

Executables can be found in the directory `build/programs'. Form the build
directory call e.g. `programs/circuit_info'
