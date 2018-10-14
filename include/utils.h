#pragma once

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