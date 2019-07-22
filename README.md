Rendergraph-GUI
====================================

This is the GUI part of rendergraph (working name).

# Getting started

## Installing dependencies

* Install [vcpkg](https://github.com/microsoft/vcpkg)
* Install Qt 5.13 (on windows, choose the msvc 2017 x64 packages)
* With vcpkg, install the zeromq, cppzmq, and rapidjson packages:
```
vcpkg install zeromq:x64-windows cppzmq:x64-windows rapidjson:x64-windows
```

## Building
```
cd <where you cloned>
mkdir build
cd build
cmake ../ -G "Visual Studio 15 2017 Win64" -DCMAKE_TOOLCHAIN_FILE=<vcpkg_install_dir>/scripts/buildsystems/vcpkg.cmake -DQt5_DIR="\lib\cmake\Qt5"
```
Replace `<vcpkg_install_dir>` with the path where you installed vcpkg, as indicated in the documentation of vcpkg. Replace `<qt_install_dir>` with the path to your Qt installation (something like `C:\Qt\5.13.0\msvc2017_64` on windows).

## Building on Linux
TODO

# Code organization

* `ext/`: third-party dependencies
* `src/`: source code
    * `gfx/`: backend-agnostic GPU graphics and compute module
    * `gfxopengl/`: OpenGL API backend for gfx
    * `ui/`: GUI-related code (Qt stuff, mostly)
    * `render/`:
    * `util/`: misc utility functions and data structures used throughout the project
