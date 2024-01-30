from conan import ConanFile, tools
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
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "include/*" ,"src/*" , "up-core-api/*"


 #   def configure(self):
     #   self.build_folder = "/projects_dev_sdv/uSpace/conan_sandbox/up-cpp"
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
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
