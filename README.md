# Uprotocol C++ Library 

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
up-cpp/0.1.2-dev

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout
```
**NOTE:** If using conan version 1.59 Ensure that the conan profile is configured to use ABI 11 (libstdc++11: New ABI.) standards according to https://docs.conan.io/en/1.60/howtos/manage_gcc_abi.html

**NOTE:** Until `up-cpp` makes it to Conan Center Index, to use the
library you must follow section below on creating conan package locally.

## How to Build 
The following steps are only required to locally build and test up-cpp, if you are a user of up-cpp, you only need to follow the _How to Use the Library_ section above. 
### Setup SDK local repository, build and test
```bash
git clone https://github.com/eclipse-uprotocol/up-cpp.git
cd up-cpp
conan source .
```

**Note that**:
```bash
conan source .
```
will clone the repo holding the project `.proto`s.

If you would rather use `ssh` to clone, you can instead invoke:
```bash
GIT_METHOD=ssh conan source .
```

### Building locally 

#### Using Conan for build

Most will probably be fine building using Conan as follows:
```bash
cd up-cpp
conan build . --build=missing -o build_testing=True
```
Places the build artifacts in `up-cpp/build/Release`.

#### Using Conan for dependencies only

If you need to, you can use Conan to only install dependencies
and scaffold out for building using CMake.
```bash
cd up-cpp
conan install . --build=missing -o build_testing=True
cmake --preset conan-release
cd build
cmake --build Release --target install -- -j
```

### Creating conan package locally 

If you need to create a release package for conan, please follow the steps below.
```bash
cd up-cpp
conan create ./conanfile_package.py --build=missing
```
Installs the `up-cpp` package in your local conan cache.
* `~/.conan` if using Conan 1.59
* `~/.conan2` if using Conan 2.X

### Updating to use new spec's `.proto`s

#### Add new release tag to `conandata.yml`:

Before:
```yaml
proto:
  # When new spec is released, replace this entry with the new spec release version and tag
  "1.5.7":
    ssh: "git@github.com:eclipse-uprotocol/up-core-api.git"
    https: "https://github.com/eclipse-uprotocol/up-core-api.git"
    sha256: "c5ad309f74db78a6d2154886404e3c13895f3dd50846ac526b5b0b3433005414"
    tag: "uprotocol-core-api-1.5.7"
```
After:
```yaml
proto:
  # When new spec is released, replace this entry with the new spec release version and tag
  "1.5.8":
    ssh: "git@github.com:eclipse-uprotocol/up-spec.git"
    https: "https://github.com/eclipse-uprotocol/up-spec.git"
    sha256: "1a1dc5d1d2bbfcda92228d6fa4b16fa3cdb5a3c96f46606209fd45bae7b993a3"
    # tentative -- replace with actual tag
    tag: "uprotocol-1.5.8"
```

#### Update `conanfile.py` to use new tag
```python
    # TODO: Upon release of up-spec 1.5.8, this will need to be updated to:
    # proto_version = "1.5.8
    proto_version = "1.5.7"
```

### One-time changes due to merging of `up-core-api` and `up-spec`

This section can be removed later after the release of `1.5.8` and
the changes performed.

Update `conanfile.py` as is commented and remove the TODOs:
```python
    # TODO: Upon release of up-spec 1.5.8, this will need to be updated to:
    # proto_containing_folder = "up-spec"
    proto_containing_folder = "up-core-api"
    # TODO: Upon release of up-spec 1.5.8, this will need to be updated to:
    # proto_subfolder = "up-core-api"
    proto_subfolder = "."
```

## Show your support

Give a ⭐️ if this project helped you!
