# uProtocol C++ Interface Library (up-cpp)

## Welcome!

This library provides the primary interface to uProtocol for uEntity
applications. The primary point of interaction for most applications will be the
layer 2 "communications" objects.

In addition to `up-cpp`, a uEntity will also need at least on transport
implementation, such as [up-transport-zenoh-cpp][zenoh-transport-repo].

*_IMPORTANT NOTE:_ This project is under active development*

## Getting Started

### Requirements:
- Compiler: GCC/G++ 11 or Clang 13
- pip : To install others
- Conan(pip) : > 2.4.2
- Cmake(apt) : > 3.22
- Ninja(pip) : Latest
- Gcovr(pip) : For UT code coverage if desired

#### Conan packages

Using the recipes found in [up-conan-recipes][conan-recipe-repo], build these
Conan packages:

1. [up-core-api][spec-repo]: `conan create --version 1.5.8 --build=missing up-core-api/developer`

**NOTE:** all `conan` commands in this document use  Conan 2.x syntax. Please
adjust accordingly when using Conan 1.x.

## How to Use the Library

To add up-cpp to your conan build dependencies, place following in your
conanfile.txt (https://docs.conan.io/2/reference/conanfile_txt.html):

```
[requires]
up-cpp/0.2.0

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout
```

**NOTE:** If using conan version 1.59 Ensure that the conan profile is
configured to use ABI 11 (libstdc++11: New ABI) standards according to
[the Conan documentation for managing gcc ABIs][conan-abi-docs].

## Building locally

The following steps are only required for developers to locally build and test
up-cpp, If you are making a project that uses up-cpp, follow the steps in the
[How to Use the Library](#how-to-use-the-library) section above.

### With Conan for dependencies

Clone the up-cpp repo to a folder named: up-cpp

```
cd up-cpp
conan install .
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j
```

Once the build completes, tests can be run with `ctest`.
Once the tests complete, the Unit Test Coverage report can be generated from the base up-cpp folder with: ./Coverage.sh

### With dependencies installed as system libraries

**TODO** Verify steps for pure cmake build without Conan.

### Creating the Conan package

See: [up-conan-recipes][conan-recipe-repo]

## Show your support

Give a ⭐️ if this project helped you!

[zenoh-transport-repo]: https://github.com/eclipse-uprotocol/up-client-zenoh-cpp
[conan-recipe-repo]: https://github.com/gregmedd/up-conan-recipes
[spec-repo]: https://github.com/eclipse-uprotocol/up-spec
[conan-abi-docs]: https://docs.conan.io/en/1.60/howtos/manage_gcc_abi.html
