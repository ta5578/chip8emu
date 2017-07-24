# Chip8 VM Emulator &copy; Tamer Aly 2017

This is an emulator for the [chip8 VM](https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description). As input, it takes a ROM (.rom) file and runs it as if it were running an a chip8 VM. 

## Building and Testing
The emulator source and tests are built using `cmake`. Unit testing is done via `UnitTest++`. To build the source the proper 
way (in an out-of-source build), create a directory and run cmake.

```
mkdir build
cd build
cmake ..
```
This will build the sources into the build directory and run unit tests as part of the build. 
Note that you must have [UnitTest++ installed](https://github.com/unittest-cpp/unittest-cpp/wiki/Building-Using-CMake) in order for the unit tests to build and run. 
If `UnitTest++` is not installed, unit tests will be skipped over and only the sources will be built.
Tests can also be run ad-hoc by typing `make test` after building the project.
The [SDL](https://wiki.libsdl.org/Installation#Installing_SDL) library is
a _required_ dependency of this project. Please make sure you have it installed
on your system.

## Creating a ROM file
ROM files can either be found online or created. You can use the [Chip8
Assembler](https://github.com/ta5578/chip8asm) I've written to write ROM files of your own.

## Running the emulator
The emulator can be run by building it and supplying it the ROM file of choice.
Here's a screenshot of the emulator running on the provided example:

```
./chip8emu myrom.rom
```
![Screenshot](/rsc/Chip8Emu.png?raw=true "Emulator Screenshot")

See the `/examples` directory for some ROM file examples.
Note that _verbose_ logging is enabled when the project is built in DEBUG mode.

## Libraries and Tools
* [CMake](https://cmake.org/)
* [UnitTest++](https://github.com/unittest-cpp/unittest-cpp/wiki/Home)
* [SDL](https://wiki.libsdl.org/FrontPage)
