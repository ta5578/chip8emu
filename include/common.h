#pragma once

#include <memory>

#ifdef NDEBUG
    #include <cstdio>


    #define LOG_IMPL(func, line, m, ...) \
            do {\
                std::fprintf(stderr, "[%s:%d] "##m##"\n", reinterpret_cast<const char*>(func), static_cast<int>(line), ##__VA_ARGS__);\
            } while (0)

    #define LOG(m, ...) LOG_IMPL(__FUNCTION__, __LINE__, m, ##__VA_ARGS__)
#else
    #define LOG(m, ...)
#endif

#define V_UNUSED(x) ((void)(x))

#define CHIP8_MEMORY_SIZE (0x1000)
#define CHIP8_REGISTER_COUNT (16)
#define CHIP8_STACK_DEPTH (16)
#define CHIP8_KEY_COUNT (16)
#define CHIP8_FONT_COUNT (80)

#define CHIP8_PIXELS_WIDTH (64)
#define CHIP8_PIXELS_HEIGHT (32)

#define CHIP8_WINDOW_SCALAR (10)

#define CHIP8_WINDOW_WIDTH (CHIP8_PIXELS_WIDTH * CHIP8_WINDOW_SCALAR)
#define CHIP8_WINDOW_HEIGHT (CHIP8_PIXELS_HEIGHT * CHIP8_WINDOW_SCALAR)

#define CHIP8_START_ADDRESS (0x0200)

using ROM = std::unique_ptr<uint8_t[]>;