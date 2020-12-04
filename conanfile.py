# 
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

from conans import ConanFile, CMake

class WatchdogConan(ConanFile):

    name = "watchdog"
    homepage = "https://github.com/ghorbanzade/watchdog"
    description = "Library and command line tool for monitoring filesystem events in a given directory on Linux"
    topics = ( "filesystem-monitoring" )
    url = "https://github.com/ghorbanzade/watchdog"
    license = "MIT"
    version ="1.0.0"
    author = "Pejman Ghorbanzade <pejman@ghorbanzade.com>"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [ True, False ],
        "with_tests": [ True, False ],
        "with_tools": [ True, False ]
    }
    default_options = {
        "shared": False,
        "with_tests": True,
        "with_tools": True
    }
    generators = "cmake_find_package"
    exports_sources = [
        "CMakeLists.txt", "include/**", "src/**", "tests/**", "tools/**"
    ]

    def source(self):
        self.run("git clone https://github.com/ghorbanzade/watchdog.git")

    def requirements(self):
        self.requires.add("fmt/7.1.2")
        if self.options.with_tools:
            self.requires.add("cxxopts/2.2.1")
            self.requires.add("spdlog/1.8.1")

    def build_requirements(self):
        if self.options.with_tests:
            self.build_requires("catch2/2.12.2")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["WATCHDOG_BUILD_TESTS"] = self.options.with_tests
        cmake.definitions["WATCHDOG_BUILD_TOOLS"] = self.options.with_tools
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def test(self):
        cmake = self._configure_cmake()
        cmake.test()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.name = "watchdog"
        self.cpp_info.components["watchdog"].names["cmake_find_package"] = "watchdog"
        self.cpp_info.components["watchdog"].libs = ["watchdog"]
        self.cpp_info.components["watchdog"].requires = ["fmt::fmt"]
