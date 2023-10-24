# Uprotocol C++ SDK 

## Welcome!

The main object of this module is to enable constructing and deconstructing uProtocol CloudEvents.

*_IMPORTANT NOTE:_ This project is under active development*

The core module contains functional factory methods for creating CloudEvents as well as functional factory methods that make it more intuitive to create URIs that are used to configure source and sink (destination) elements in the uProtocol CloudEvents.

This module contains the data model structures as well as core functionality for building uProtocol CloudEvents and URIs for sink and source attributes.

The SDKs are then used by the code generators to auto-populate service stubs with generated code that builds CloudEvents. For more information on auto-generating service stubs, please refer to uProtocol Main Project

## Getting Started
### Requirements:
- Compiler: GCC/G++ 11 or Clang 13
- vcpkg
- Ubuntu 22.04
- cgreen testing library

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
```
#### Install cgreen testing library

In this package we're using the cgreen testing library.

```
 $ git clone https://github.com/cgreen-devs/cgreen.git
 $ cd cgreen
 $ make
 $ sudo make install
 ```

It should appear in /usr/local/lib and in /usr/local/include

#### Microsoft vcpkg installation

Now for C++ depedencies we're using Microsoft vcpkg manager.
```
$ cd $HOME
$ git clone https://github.com/Microsoft/vcpkg.git
$ .\vcpkg\bootstrap-vcpkg.bat
$ export PATH=$PATH:~/vcpkg
$ vcpkg install openssl
$ vcpkg install rapidjson
$ vcpkg install protobuf protobuf[zlib]
$ vcpkg install spdlog
```

#### Setup SDK local repository, build and test
```
$ git clone https://github.com/eclipse-uprotocol/uprotocol-sdk-cpp.git
$ cd uprotocol-sdk-cpp
$ code .

```
Using VSCode with the CMake & CMake Tools plugins, following steps will be done automatically when the project is opened in VSCode. Build and test can be triggered using the extension.

Please refer the following link for the CMake Tools extension:
https://code.visualstudio.com/docs/cpp/CMake-linux

If the CMake Tools plugin is not configured to auto generate, then manually run cmake and build using the following commands.
```
$ /usr/bin/cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S . -B build -G Ninja
$ cd build
$ ninja
$ ninja test
```
Now you should see formatting the code with clang-format, building and running the unit tests.


## Show your support

Give a ⭐️ if this project helped you!
