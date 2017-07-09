#pragma once

#include <iostream>
#include <cstdint>

#define CHIP8_MEMORY_SIZE (4096)
#define CHIP8_REGISTER_COUNT (16)
#define CHIP8_STACK_DEPTH (16)
#define CHIP8_KEY_COUNT (16)

#define CHIP8_WINDOW_WIDTH (640)
#define CHIP8_WINDOW_HEIGHT (480)

#define CHIP8_PIXELS_WIDTH (64)
#define CHIP8_PIXELS_HEIGHT (32)


#define VLOG(x, y) do {\
        if ((x).verbose) {\
            std::cout << y << '\n';\
        }\
    } while(0)

#define V_UNUSED(x) ((void)(x))


struct AsmOpts {
    const char *in_file;
    bool show_help, verbose;

    AsmOpts(const char *in = "", bool show_help = false, bool verbose = false) :
        in_file(in), show_help(show_help), verbose(verbose) {}
};