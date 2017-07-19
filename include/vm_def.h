#pragma once

#include <iostream>

#define CHIP8_MEMORY_SIZE (4096)
#define CHIP8_REGISTER_COUNT (16)
#define CHIP8_STACK_DEPTH (16)
#define CHIP8_KEY_COUNT (16)
#define CHIP8_FONT_COUNT (80)

#define CHIP8_WINDOW_WIDTH (640)
#define CHIP8_WINDOW_HEIGHT (480)

#define CHIP8_PIXELS_WIDTH (64)
#define CHIP8_PIXELS_HEIGHT (32)

#define CHIP8_START_ADDRESS (0x0200)

#ifndef NDEBUG
    #define LOG(x) std::cout << x << "\n"
#else
    #define LOG(x)
#endif

#define V_UNUSED(x) ((void)(x))