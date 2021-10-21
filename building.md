# Binary bakery - Building from source
===========================

This document has instructions on how to build Binary bakery :cookie: from source. Note that it only covers the build of library itself and is mostly meant for contributors and/or power users.
Other should follow the user instructions. See the [Readme](readme.md) for instructions.

## Pre-requisites

Recommended way to install the dependencies is using [vcpkg](https://vcpkg.io/en/index.html).

Quick start on Windows:
```powershell
git clone https://github.com/microsoft/vcpkg build/vcpkg
.\build\vcpkg\bootstrap-vcpkg.bat
.\build\vcpkg\vcpkg install zstd lz4 tomlplusplus fmt stb doctest --triplet=x64-windows-static
```

Ubuntu
```bash
git clone https://github.com/microsoft/vcpkg build/vcpkg
./build/vcpkg/bootstrap-vcpkg.sh
./build/vcpkg/vcpkg install zstd lz4 tomlplusplus fmt stb doctest --triplet=x64-linux
```

## Configure \& Build

Configuration step will require the path to vcpkg.
Windows
```console
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%cd%/build/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_INSTALL_PREFIX=install
```

Ubuntu
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux
```

Compile all
```console
cmake --build build --config Release --target Install
```

Run the tests
Windows
```console
.\build\tests\Release\tests.exe
```
Ubuntu
```bash
./build/tests/tests
```

## Contribute
- Install scripts
- CI/CD
- CMake functions to automate resource file generation