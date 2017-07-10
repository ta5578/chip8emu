#pragma once

#include <cstdlib>
#include <cstdio>
#include <memory>
#include <string>

inline size_t fsize(FILE *fp)
{
    /* Get where we are in the file first */
    size_t pos = std::ftell(fp);

    /* Now go to the end */
    std::fseek(fp, 0, SEEK_END);

    /* Get the size of the file now (end - beginning) */
    size_t fsize = std::ftell(fp);

    /* Rewind to where we were */
    std::fseek(fp, pos, SEEK_SET);

    return fsize;
}

inline std::unique_ptr<uint8_t> read_rom(const char *path, size_t *size)
{
    std::FILE *rom = std::fopen(path, "rb");
    if (!rom) { return nullptr; }

    size_t file_size = fsize(rom);
    uint8_t *mem = static_cast<uint8_t*>(std::malloc(file_size));
    if (!mem) { return nullptr; }

    std::fread(mem, sizeof(*mem), file_size, rom);
    std::fclose(rom);

    if (size) { *size = file_size; }
    return std::unique_ptr<uint8_t>(mem);
}

inline std::string to_str(uint16_t num)
{
    char buf[sizeof(num)];
    int i = 0;
    while (num) {
        auto d = num % 10;
        buf[i++] = d + '0';
        num /= 10;
    }
    std::string s;
    for (i = i - 1; i >= 0; --i) {
        s += buf[i];
    }
    return s;
}