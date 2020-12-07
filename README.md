# COMP3000-Automatic-Vibration-Alarm-Adjustments


## Building

### Windows
For installing package managers run below commands in the Middleware folder
A reload CMake project command may be necessary after this.

$ conan install . -s build_type=Debug --install-folder=cmake-build-debug
$ conan install . -s build_type=Release --install-folder=cmake-build-release

### MacOS