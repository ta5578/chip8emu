#pragma once

#include <string>
#include <cctype>
#include <cstdio>
#include "common.h"

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
            val += static_cast<uint16_t>(cl - 'a' + 10);
        } else if (std::isdigit(cl)) {
            val += static_cast<uint16_t>(cl - '0');
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
            buf[i] = static_cast<char>('A' + (d - 10));
        } else {
            buf[i] = static_cast<char>('0' + d);
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

inline ROM read_bin_file(const char* filePath)
{
    std::FILE* rom = std::fopen(filePath, "rb");
    if (rom == nullptr) {
        return nullptr;
    }

    std::fseek(rom, 0, SEEK_END);
    auto fileSize = std::ftell(rom);
    std::fseek(rom, 0, SEEK_SET);

    auto memory = std::make_unique<uint8_t[]>(fileSize);
    
    std::fread(memory.get(), sizeof(uint8_t), fileSize, rom);
    std::fclose(rom);

    return memory;
}