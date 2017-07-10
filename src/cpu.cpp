#include "cpu.h"
#include <cstring>
#include <iostream>
#include "utils.h"

CPU::CPU(std::unique_ptr<uint8_t> rom, size_t len) : opcode(0), index(0), pc(0x200), delay_timer(0),
    sound_timer(0), sp(0)
{
    /* Load the rom into memory */
    auto mem = rom.get();
    for (size_t i = 0; i < len; ++i) {
        memory[i + 0x200] = mem[i];
    }
    /* Reading in little endian order */
    for (size_t i = 0; i < len; i += 2) {
        std::cout << to_str(memory[i + 0x200] << 8 | memory[i + 0x201]) << " ";
    }

    std::memset(V, 0, sizeof(V));
    std::memset(gfx, 0, sizeof(gfx));
    std::memset(stack, 0, sizeof(stack));
    std::memset(key, 0, sizeof(key));
}

CPU::~CPU() {}

void CPU::emulate_cycle()
{
    uint16_t op = next();
    // decode and execute
    pc += 2;
}

bool CPU::is_draw_set() const
{
    return false;
}

void CPU::draw()
{

}


uint16_t CPU::next()
{
    return memory[pc] << 8 | memory[pc + 1];
}