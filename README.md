# SQLite

SQLite is a C-language library that implements a small, fast, self-contained,
high-reliability, full-featured, SQL database engine. SQLite is the most used
database engine in the world. SQLite is built into all mobile phones and most
computers and comes bundled inside countless other applications that people use
every day.

## Usage

This project builds the following:

* `SQLite` -- a shared library containing the `SQLite` SQL database engine,
  suitable for linking into any C or C++ program.
* `sqlite3` -- a command-line shell program which can be used to interact with
  `SQLite` in a terminal.
* `SQLPlay` -- a small playground application used to demonstrate `SQLite` and
  experiment with its features.

## Supported platforms / recommended toolchains

This project builds portable libraries and programs which depend only on the
C++11 compiler, the C and C++ standard libraries, and other C++11 libraries
with similar dependencies, so it should be supported on almost any platform.
The following are recommended toolchains for popular platforms.

* Windows -- [Visual Studio](https://www.visualstudio.com/) (Microsoft Visual
  C++)
* Linux -- clang or gcc
* MacOS -- Xcode (clang)

## Building

This library may stand alone or be included in a larger project.  It uses and
maybe integrated into larger systems which use [CMake](https://cmake.org/),
which allows build systems to be constructed in various environments and using
various tools.

There are two distinct steps in the build process:

1. Generation of the build system, using CMake
2. Compiling, linking, etc., using CMake-compatible toolchain

### Prerequisites

* [CMake](https://cmake.org/) version 3.8 or newer
* C++11 toolchain compatible with CMake for your development platform (e.g.
  [Visual Studio](https://www.visualstudio.com/) on Windows)

### Build system generation

Generate the build system using [CMake](https://cmake.org/) from the solution
root.  For example:

```bash
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A "x64" ..
```

### Compiling, linking, et cetera

Either use [CMake](https://cmake.org/) or your toolchain's IDE to build.
For [CMake](https://cmake.org/):

```bash
cd build
cmake --build . --config Release
```
