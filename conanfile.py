from conan import ConanFile, tools
from conans import ConanFile, CMake

from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
import shutil

class up_cpp(ConanFile):
    name = "up-cpp"
    version = "0.1"

    # Optional metadata
    license = "Apache-2.0 license"
    url = "https://github.com/eclipse-uprotocol/up-cpp-api"
    description = "This module contains the data model structures as well as core functionality for building uProtocol"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [False, False]}
    default_options = {"shared": True, "fPIC": False}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "conaninfo/*", "include/*" ,"src/*" , "test/*"
    requires = [
        "spdlog/1.13.0",
        "fmt/10.2.1",
        "gtest/1.14.0"]

    generators = "CMakeDeps"

    def source(self):
        self.run("git clone https://github.com/eclipse-uprotocol/up-core-api.git")

    def requirements(self):
        self.requires("protobuf/3.21.12")
        self.requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.preprocessor_definitions["CMAKE_CXX_STANDARD"] = "17"
        tc.preprocessor_definitions["CMAKE_POSITION_INDEPENDENT_CODE"] = "ON"
        tc.preprocessor_definitions["CMAKE_CXX_FLAGS"] = "-MD -MT"
        #tc.preprocessor_definitions["GLIBCXX_USE_CXX11_ABI"] = "1"
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
