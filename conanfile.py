from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import apply_conandata_patches, copy, export_conandata_patches, get, replace_in_file, rm, rmdir
import os

class UpCpp(ConanFile):
    name = "up-cpp"
    package_type = "library"
    license = "Apache-2.0 license"
    homepage = "https://github.com/eclipse-uprotocol"
    url = "https://github.com/conan-io/conan-center-index"
    description = "This module contains the data model structures as well as core functionality for building uProtocol"
    topics = ("utransport sdk", "transport")
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    conan_version = None
    generators = "CMakeToolchain", "CMakeDeps", "pkg_config", "PkgConfigDeps", "VirtualRunEnv", "VirtualBuildEnv", "cmake_find_package"
    version = "0.1"
    exports_sources = "CMakeLists.txt", "conaninfo/*", "include/*" ,"src/*" , "test/*"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_testing": [True, False],
        "build_unbundled": [True, False],
        "build_cross_compiling": [True, False],
    }

    default_options = {
        "shared": False,
        "fPIC": False,
        "build_testing": False,
        "build_unbundled": False,
        "build_cross_compiling": False,
    }
    up_core_api_version = "1.5.5"
    
    def source(self):
        if not self.options.build_unbundled:
            self.run(f"git clone --branch uprotocol-core-api-{self.up_core_api_version} https://github.com/eclipse-uprotocol/up-core-api.git")
        
    def requirements(self):
        if not self.options.build_cross_compiling:
            self.requires("protobuf/3.21.12")
        else:
            self.requires("protobuf/3.21.12@cross/cross")
        self.requires("spdlog/1.13.0")
        if self.options.build_testing:
            self.requires("gtest/1.14.0")
        if self.options.build_unbundled:
            self.requires("up-core-api/{}".format(self.up_core_api_version))
        

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = self.options.build_testing
        tc.variables["BUILD_UNBUNDLED"] = self.options.build_unbundled
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["up-cpp"]
