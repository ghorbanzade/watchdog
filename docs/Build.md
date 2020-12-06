# Build Instructions

## Before You Build

This project uses Conan and CMake as its build toolchain. It has formal
support for `gcc-9` and `clang-9`. The code makes use of Linux-specific
function APIs. While you may be able to build it on non-Linux platforms,
you will need a Linux platform to run any of the application components.

### Required Build Dependencies:

* Reasonably recent version of `gcc` or `clang`
* `Bash` (*v4.4 or higher*)
* `Conan`
* `CMake`

### Optional Build Dependencies:

* `ccache`: the build script automatically detects if `ccache` is installed
  and configures the build system to use it to speed up the builds.
* `ninja`: the build script prefers checks if `ninja` is installed and prefers
  it over `make`.
* `doxygen`: only required for building project documentation.

### Third-party Libraries

This software makes use of the following third-party libraries. These
dependencies are entirely managed by `conan`. Given that they are all
header-only libraries, it is easy to integrate with them if we have
reasons not to use `conan`.

* fmt (v7.1.2): for simple text formatting
* spdlog (1.8.1): for basic logging in `watchdogd`
* cxxopts (v2.2.1): for managing command line arguments of `watchdog`
* catch2 (v2.12.2): for unit-testing

## Building Watchdog

For convenience, we provide `build.sh` (required bash v4.4 or higher)
which can be run to pull third-party libraries using Conan and build
the project using CMake. The build script can also be used to build
project documentation, and to build and run project unit-tests.

If you prefer not to use `build.sh`, checkout the implementation of `build.sh`
or our Github Actions CI workflow to learn how to run `conan` and `cmake`.

```txt
usage: build.sh [ -h | --long-options]
  -h, --help                shows this message
  --debug                   enable debug logs

  --with-tests              include unit-tests in build

  --docs                    build library documentation
  --lint                    lint source code
  --clear                   remove build artifacts
  --test                    test watchdog library
```

Run the following command from the project root directory to build all
Watchdog components including the unit-tests:

```bash
./build.sh --with-tests
```

Once you build all components, you can use the following command to run
Watchdog unit tests using `ctest`:

```bash
./build.sh --test
```

## Build Artifacts

The most useful build artifacts (except the unit-tests) are generated in
the `./local/dist` directory by default. You can configure this behavior
via the top-level CMakeLists.txt file. The main build components are listed
below:

* `watchdogd` executable meant to be run as service.

* `watchdog` command line tool for interacting with the service.

* `libwatchdog.a` is the static library that includes the core logic used
  by `watchdog` and `watchdogd` executables.

## Installing

By default, the build script installs all component in the `./local/dist`.
We leave it to users to copy this directory into their system directories,
if they liked to do so.

Watchdog has a daemon that can be run either as a standalone executable
or a `systemctl` service. The instructions below help you install `watchdogd`
as a service:

* Install Watchdog system service unit.

    ```bash
    cp ./config/service-unit.txt /etc/systemd/system/watchdog.service
    ```

* Reload `systemd` to load the system service unit.

    ```bash
    sudo systemctl daemon-reload
    ```

* Start the service.

    ```bash
    sudo systemctl start watchdog.service
    ```

    You can undo this anytime via `sudo systemctl stop watchdog.service`.

* Enable the service to start at startup.

    ```bash
    sudo systemctl enable watchdog.service
    ```

## Packaging

The build script automatically creates a Conan package for the watchdog
library so that it can be used as a thirdparty dependency. You can use the
content of the `local/dist` directory to manually package the library and
its header files for other build ecosystems.

**Note**: `watchdog` and `watchdogd` executables are intentionally left out
of the `conan` package.
