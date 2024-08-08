from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy
from os.path import join, sep


class SPeMPECMakeGameRecipe(ConanFile):
    name = "SPeMPECMakeGame"
    version = "1.0"
    package_type = "application"

    # Optional metadata
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of the package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False], 
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True
    }

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "Source/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def requirements(self):
        self.requires("hobgoblin/0.1.0@jbatnozic/stable")
        # Add more dependencies here...

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        # cmake_layout(self)
        # Use most basic layout for now
        self.folders.source = "."
        self.folders.build  = ""

    def generate(self):
        # Import dynamic libraries
        game_bindir = join(self.build_folder, "bin", str(self.settings.build_type))
        for dep in self.dependencies.values():
            bindirs = dep.cpp_info.bindirs
            # about the 'if' condition:
            # - check if bindirs is not undefined;
            # - some packages wrongly define it as a relative path, so to avoid recursively
            #   copying our own bindir, we check that it contains the OS's separator (if it
            #   has at least 1, then we know it's an absolute path).
            if (len(bindirs) > 0 and (sep in bindirs[0])):
                copy(self, "*.dylib", bindirs[0], game_bindir)
                copy(self, "*.dll",   bindirs[0], game_bindir)
        
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        # Only needed if you're building a library and not an application
        pass

    def package_info(self):
        # Only needed if you're building a library and not an application
        pass
