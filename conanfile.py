from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import collect_libs, apply_conandata_patches, copy, export_conandata_patches, get, replace_in_file, rm, rmdir
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
    generators = "CMakeDeps", "PkgConfigDeps", "VirtualRunEnv", "VirtualBuildEnv"
    # version = "0.1.1-dev" # set version from command line
    exports_sources = "CMakeLists.txt", "up-core-api/*", "include/*" ,"src/*" , "test/*", "cmake/*"

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
        
    def requirements(self):
        self.requires("protobuf/3.21.12" + ("@cross/cross" if self.options.build_cross_compiling else ""))
        self.requires("spdlog/1.13.0")
        if self.options.build_testing:
            self.requires("gtest/1.14.0")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = self.options.build_testing
        tc.variables["BUILD_UNBUNDLED"] = self.options.build_unbundled
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.generate()

    def build(self):
        if os.environ.get("BUILD_DIR_FULL_PATH") is not None:
            return
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        if os.environ.get("BUILD_DIR_FULL_PATH") is not None:
            self.output.info("BUILD_DIR_FULL_PATH: " + os.environ.get("BUILD_DIR_FULL_PATH"))
            build_folder_path = os.environ.get("BUILD_DIR_FULL_PATH")
            self.output.info("BUILD_DIR_FULL_PATH: " + build_folder_path)
            self.copy(pattern='*', dst='.', src=('%s/install') % (build_folder_path), symlinks=True, ignore_case=False)
        else:
            cmake = CMake(self)
            cmake.install()

    def package_info(self):
        if os.environ.get("BUILD_DIR_FULL_PATH") is not None:
            self.cpp_info.set_property("cmake_file_name", "up-cpp")
            self.cpp_info.set_property("cmake_target_name", "up-cpp::up-cpp")
            self.cpp_info.set_property("pkg_config_name", "up-cpp")
            self.cpp_info.libs = collect_libs(self)

            self.cpp_info.set_property("cmake_target_name", "up-cpp::up-cpp")
            self.cpp_info.requires = ["spdlog::spdlog", "protobuf::protobuf"]

            self.cpp_info.names["cmake_find_package"] = "up-cpp"
            self.cpp_info.names["cmake_find_package_multi"] = "up-cpp"

        else:
            self.cpp_info.libs = ["up-cpp"]
