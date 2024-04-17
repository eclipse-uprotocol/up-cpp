from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.scm import Git
from conan.tools.files import rmdir
import os

class UpCppLocal(ConanFile):
    name = "up-cpp"
    package_type = "library"
    license = "Apache-2.0 license"
    homepage = "https://github.com/eclipse-uprotocol"
    url = "https://github.com/conan-io/conan-center-index"
    description = "This module contains the data model structures as well as core functionality for building uProtocol"
    topics = ("uProtocol", "SDV", "middleware")
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    conan_version = None
    generators = "CMakeDeps", "PkgConfigDeps", "VirtualRunEnv", "VirtualBuildEnv"
    version = "0.1.2-dev"
    # TODO: Upon release of up-spec 1.5.8, this will need to be updated to:
    # proto_version = "1.5.8
    proto_version = "1.5.7"
    # TODO: Upon release of up-spec 1.5.8, this will need to be updated to:
    # proto_containing_folder = "up-spec"
    proto_containing_folder = "up-core-api"
    # TODO: Upon release of up-spec 1.5.8, this will need to be updated to:
    # proto_subfolder = "up-core-api"
    proto_subfolder = "."
    proto_folder = "uprotocol"
    exports_sources = "CMakeLists.txt", "up-core-api/*", "include/*" ,"src/*" , "test/*", "cmake/*"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_testing": [True, False],
        "build_unbundled": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": False,
        "build_testing": False,
        "build_unbundled": False,
    }

    def layout(self):
        cmake_layout(self)
        self.folders.build = os.path.join("build", str(self.settings.build_type))
        self.folders.install = "install"

    def source(self):
        # we clone protos from the appropriate repo at the appropriate tag to our local root to build
        rmdir(self, self.proto_containing_folder)

        git = Git(self)
        git_method = os.getenv("GIT_METHOD", "https")
        git.clone(self.conan_data["proto"][self.proto_version][git_method], target=self.proto_containing_folder)
        git.folder=self.proto_containing_folder
        git.checkout(self.conan_data["proto"][self.proto_version]["tag"])

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["PROTO_PATH"] = os.path.join(self.proto_containing_folder, os.path.join(self.proto_subfolder, self.proto_folder))
        tc.variables["BUILD_TESTING"] = self.options.build_testing
        tc.variables["BUILD_UNBUNDLED"] = self.options.build_unbundled
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["CMAKE_INSTALL_PREFIX"] = self.folders.install
        tc.generate()

    def requirements(self):
        self.requires("protobuf/3.21.12")
        self.requires("spdlog/1.13.0")
        if self.options.build_testing:
            self.requires("gtest/1.14.0")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["up-cpp"]
