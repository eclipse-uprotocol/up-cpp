# Uprotocol C++ SDK 

## Welcome!

This is the C++ library that extends up-core-api to provide serializers, validators, and language specific interface definitions for uProtocol. 

*_IMPORTANT NOTE:_ This project is under active development*

The core functional factory methods that make it more intuitive to create URIs that are used to configure source and sink (destination) elements in the uProtocol and additional helper methods.


## Getting Started

The library is built and packaged using conan dependency management. 

### Requirements:
- Compiler: GCC/G++ 11 or Clang 13
- Conan : 1.59 or latest 2.X
- Ubuntu 22.04

## How to Use the Library
To add up-cpp to your conan build dependencies, simply add the following to your conanfile.txt:
```
[requires]
up-cpp/0.1

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout

```
**NOTE:** If using conan version 1.59 Ensure that the conan profile is configured to use ABI 11 (libstdc++11: New ABI.) standards according to https://docs.conan.io/en/1.60/howtos/manage_gcc_abi.html

## How to Build 
The following steps are only required to locally build and test up-cpp, if you are a user of up-cpp, you only need to follow the _How to Use the Library_ section above. 
### Setup SDK local repository, build and test
```
$ git clone -b uprotocol-core-api-1.5.5 https://github.com/eclipse-uprotocol/up-core-api.git
$ git clone https://github.com/eclipse-uprotocol/up-cpp.git
```

### Building locally 
```
$ cd up-cpp
$ conan install conaninfo/  --output-folder=.
$ cd build/Release
$ cmake ../../ -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
$ make -j 
```

### Creating conan package locally 
If you need to create a release package for conan, please follow the steps below.

```
$ cd up-cpp
$ conan create . --build=missing
```

## Show your support

Give a ⭐️ if this project helped you!
