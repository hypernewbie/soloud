Vsol - AI Slop SoLoud Fork
======

VsoLoud is an utterly YOLO (friendly) fork of Soloud, with vibe-coded modernisations.
The aim is to preserve the API but keep it minimum level of "maintained enough" via AI coding.

Planned Changes include:
* CMake 4 build
* Simplify backends to null and miniaudio only
* Update to latest miniaudio
* Unit tests + CI, hopefully

Vsol is work-in-progress.

> WARNING: VSoLoud contains vibe coded AI changes, albeit with professional software engineer review. Read or use at your own risk of sanity.

Original SoLoud Description
-------------------
SoLoud is an easy to use, free, portable c/c++ audio engine for games.

![ScreenShot](https://raw.github.com/jarikomppa/soloud/master/soloud.png)

Zlib/LibPng licensed. Portable. Easy.

Official site with documentation can be found at:
http://soloud-audio.com

> SoLoud is awesome. Please support the original project developer.

Building with CMake
-------------------

SoLoud can be built using CMake (3.16+).

### Basic Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Building with Clang and libc++

To build using Clang and libc++ on Linux:

```bash
export CC=clang
export CXX=clang++
export CXXFLAGS="-stdlib=libc++"
export LDFLAGS="-stdlib=libc++ -lc++abi"

cmake -B build -S . -DSOLOUD_BUILD_DEMOS=ON
cmake --build build
```

### Options

You can enable/disable specific backends or features using standard CMake options:

- `-DSOLOUD_BACKEND_ALSA=ON/OFF` (Default: ON)
- `-DSOLOUD_BACKEND_SDL2=ON/OFF` (Default: OFF)
- `-DSOLOUD_BACKEND_OSS=ON/OFF` (Default: ON)
- `-DSOLOUD_BUILD_DEMOS=ON/OFF` (Default: OFF)