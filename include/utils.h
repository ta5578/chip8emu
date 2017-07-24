#pragma once

#include <string>
#include <cctype>

#ifndef NDEBUG
    #include <iostream>
#endif

#define CHIP8_MEMORY_SIZE (4096)
#define CHIP8_REGISTER_COUNT (16)
#define CHIP8_STACK_DEPTH (16)
#define CHIP8_KEY_COUNT (16)
#define CHIP8_FONT_COUNT (80)

#define CHIP8_PIXELS_WIDTH (64)
#define CHIP8_PIXELS_HEIGHT (32)

#define CHIP8_WINDOW_FACTOR (10)

#define CHIP8_WINDOW_WIDTH (CHIP8_PIXELS_WIDTH * CHIP8_WINDOW_FACTOR)
#define CHIP8_WINDOW_HEIGHT (CHIP8_PIXELS_HEIGHT * CHIP8_WINDOW_FACTOR)

#define CHIP8_START_ADDRESS (0x0200)

#ifndef NDEBUG
    #define LOG(x) std::cout << x << "\n"
#else
    #define LOG(x)
#endif

#define V_UNUSED(x) ((void)(x))

inline uint16_t endi(uint16_t num)
{
    #if (__BYTE_ORDER == __LITTLE_ENDIAN)
        return (num & 0x00FF) << 8 | (num & 0xFF00) >> 8;
    #elif (__BYTE_ORDER == __BIG_ENDIAN)
        return num;
    #else
        #error "Couldn't determine endianess!"
    #endif
}

/* Given ^\$[0-9]+ string and return a hexadecimal representation */
inline uint16_t to_hex(const std::string& s)
{
    if (s.empty()) {
        return 0;
    }

    uint16_t val = 0x0000;
    for (const char c : s) {
        val *= 16;
        auto cl = std::tolower(c);
        if (cl >= 'a' && cl <= 'f') {
            val += (cl - 'a' + 10);
        } else if (std::isdigit(cl)) {
            val += (cl - '0');
        }
    }
    return val;
}

inline std::string from_hex(uint16_t num)
{
    if (num == 0) {
        return "0x0000";
    }

    char buf[sizeof(num)];

    int i = 0;
    while (num) {
        uint16_t d = num % 16;
        if (d >= 10 && d <= 15) {
            buf[i] = ('A' + (d - 10));
        } else {
            buf[i] = ('0' + d);
        }
        num /= 16;
        ++i;
    }

    std::string s = "0x";
    for (i = i - 1; i >= 0; --i) {
        s += buf[i];
    }
    return s;
}