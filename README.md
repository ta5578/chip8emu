# Chip8 VM Emulator

This is an emulator for the [chip8 VM](https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description). As input, it takes a ROM (.rom) file and runs it as if it were running an a chip8 VM.

## Building and Testing
The emulator source and tests are built using `cmake`. By default, the project is built in RELEASE mode. To build the project in debug mode, pass "debug" as the CMAKE_BUILD_TYPE.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=debug ..
```

[SDL](https://wiki.libsdl.org/Installation#Installing_SDL) is a _required_ dependency of the project.
For convenience, the headers have been included in the project and the libraries statically linked. This is allowed
under SDL's expanded zlib license.

## Creating a ROM file
ROM files can either be found online or created. You can use the [Chip8
Assembler](https://github.com/tamerfrombk/chip8asm) I've written to assemble ROM files of your own. See that project's
README for instructions on how to assemble your own ROMs.

## Running the emulator
The emulator can be run by building it and supplying it the ROM file of choice.
Here's a screenshot of the emulator running on the 'stars' example:

```
./chip8emu stars.ch8
```

![](/rsc/stars.gif?raw=true "Stars Running On Emulator")

See the `/examples` directory for some ROM file examples.

Note that _verbose_ logging is enabled when the project is built in DEBUG mode.

## Credits
* [CMake](https://cmake.org/)
* [SDL](https://wiki.libsdl.org/FrontPage)
* Thanks to Sergey Naydenov for the 'stars' ROM in the examples.
