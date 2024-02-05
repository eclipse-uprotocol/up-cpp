# Uprotocol C++ SDK 

## Welcome!

The main object of this module is to enable the work with the up-client-zenoh-cpp library , the module provides the interface definitions and the utilities needed for up-client-zenoh-cpp

*_IMPORTANT NOTE:_ This project is under active development*

The core functional factory methods that make it more intuitive to create URIs that are used to configure source and sink (destination) elements in the uProtocol and additional helper methods.

This module contains the data model structures as well as core functionality for building uProtocol and URIs for sink and source attributes.

The SDKs are then used by the code generators to auto-populate service stubs with generated code that builds CloudEvents. For more information on auto-generating service stubs, please refer to uProtocol Main Project

## Getting Started
### Requirements:
- Compiler: GCC/G++ 11 or Clang 13
- Conan : 1.59
- Ubuntu 22.04

#### Ubuntu dependencies

At first to make it working, you have to install some linux libraries as dependencies
```
$ sudo apt-get install build-essential \
      libbsd-dev \
      make \
      ninja-build \
      pkg-config \
      clang-format \
      cmake \
      uuid \
      gcc-11 \
      g++-11 \
      lcov \
      uuid-dev \
      openssl \ 
      libssl-dev
$ pip3 install conan==1.59
```

#### Setup SDK local repository, build and test
```
$ git clone https://github.com/eclipse-uprotocol/up-core-api.git
$ git clone https://github.com/eclipse-uprotocol/up-cpp.git
```

#### Building locally 
```
$ cd up-cpp
$ mkdir build
$ cd build
$ conan install ../conaninfo
$ cmake ../
$ make -j 
```

#### Creating conan package locally 
ensure that the conan profile is configured to use ABI 11 (libstdc++11: New ABI.) standards according to https://docs.conan.io/en/1.60/howtos/manage_gcc_abi.html
```
$ cd up-cpp
$ conan create . --build=missing
```

## Show your support

Give a ⭐️ if this project helped you!
