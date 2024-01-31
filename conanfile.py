from conan import ConanFile, tools
from conans import ConanFile, CMake

from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
import shutil

class up_core_api(ConanFile):
    name = "up-cpp"
    version = "0.1"

    # Optional metadata
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "https://github.com/eclipse-uprotocol/up-cpp-api"
    description = "This project contains the core data models (UUri, UAttributes, etc..) and core services definitions (uDiscovery, uSubscription, uTwin) of uProtocol."

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [False, False]}
    default_options = {"shared": True, "fPIC": False}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "conaninfo/*", "include/*" ,"src/*" , "up-core-api/*", "test/*"
    requires = [
        "spdlog/1.13.0",
        "fmt/10.2.1",
        "rapidjson/cci.20230929",
        "gtest/1.14.0"
    ]
    generators = "CMakeDeps"

    def requirements(self):
        self.requires("protobuf/3.21.12")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.preprocessor_definitions["CMAKE_CXX_STANDARD"] = "17"
        tc.preprocessor_definitions["CMAKE_POSITION_INDEPENDENT_CODE"] = "ON"
        tc.preprocessor_definitions["CMAKE_CXX_FLAGS"] = "-MD -MT"
         
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["up-cpp", "protobuf::protobuf", "spdlog::spdlog", "fmt::fmt"]
