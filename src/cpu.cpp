#include "cpu.h"
#include <cstring>
#include <iostream>
#include "utils.h"

CPU::CPU(std::unique_ptr<uint8_t> rom, size_t len) : sp(0), opcode(0), index(0),
    pc(0x200), delay_timer(0), sound_timer(0)
{
    /* Load the rom into memory */
    uint8_t *mem = rom.get();
    std::memcpy(memory + CHIP8_START_ADDRESS, mem, len);

    std::memset(V, 0, sizeof(V));
    std::memset(gfx, 0, sizeof(gfx));
    std::memset(stack, 0, sizeof(stack));
    std::memset(key, 0, sizeof(key));
}

CPU::~CPU() {}

void CPU::emulate_cycle()
{
    uint16_t op = next();
    decode(op);
    pc += 2;
}

bool CPU::is_draw_set() const
{
    return false;
}

void CPU::draw()
{

}


void CPU::decode(uint16_t op)
{
    uint16_t high = op & 0xF000;
    switch (high) {
    case 0x0000: {
        uint16_t low = op & 0x0FFF;
        switch (low) {
        case 0x00E0:
            /* Clear */
            break;
        case 0x00EE:
            /* RET */
            pc = stack[--sp];
            break;
        }
    break;
    } /* End 0x0000 */
    case 0x1000:
        /* JMP */
        pc = op & 0x0FFF;
    break;
    case 0x2000:
        /* CALL */
        stack[sp++] = pc;
        pc = op & 0x0FFF;
    break;
    case 0x3000:
        /* SKE */
        if (V[(op & 0x0F00) >> 8] == (op & 0x00FF)) {
            pc += 2;
        }
    break;
    case 0x4000:
        /* SKNE */
        if (V[(op & 0x0F00) >> 8] != (op & 0x00FF)) {
            pc += 2;
        }
    break;
    case 0x5000:
        /* SKRE */
        if (V[(op & 0x0F00) >> 8] == V[(op & 0x00F0) >> 4]) {
            pc += 2;
        }
    break;
    case 0x6000:
        /* LOAD */
        V[(op & 0x0F00) >> 8] = op & 0x00FF;
    break;
    case 0x7000:
        /* ADD */
        V[(op & 0x0F00) >> 8] += op & 0x00FF;
    break;
    case 0x8000: {
        uint16_t low = op & 0x000F;
        switch (low) {
        case 0x0000:
            /* ASN */
            V[(op & 0x00F0) >> 4] = V[(op & 0x0F00) >> 8];
        break;
        case 0x0001:
            /* OR */
            V[(op & 0x0F00) >> 8] |= V[(op & 0x00F0) >> 4];
        break;
        case 0x0002:
            /* AND */
            V[(op & 0x0F00) >> 8] &= V[(op & 0x00F0) >> 4];
        break;
        case 0x0003:
            /* XOR */
            V[(op & 0x0F00) >> 8] ^= V[(op & 0x00F0) >> 4];
        break;
        case 0x0004:
            /* RADD */
            if(V[(op & 0x00F0) >> 4] > (0xFF - V[(op & 0x0F00) >> 8]))
                V[0xF] = 1; /* carry */
            else
                V[0xF] = 0;
            V[(op & 0x0F00) >> 8] += V[(op & 0x00F0) >> 4];
        break;
        case 0x0005:
            /* SUB */
            if(V[(op & 0x00F0) >> 4] > (0xFF - V[(op & 0x0F00) >> 8]))
                V[0xF] = 1; /* borrow */
            else
                V[0xF] = 0;
            V[(op & 0x0F00) >> 8] -= V[(op & 0x00F0) >> 4];
        break;
        case 0x0006:
            /* SHR */
            V[0xF] = V[(op & 0x0F00) >> 8] & 0x1;
            V[(op * 0x0F00) >> 8] >>= 1;
        break;
        case 0x0007:
            /* RSUB */
            V[(op & 0x0F00) >> 8] = V[(op & 0x00F0) >> 4] - V[(op & 0x0F00) >> 8];
        break;
        case 0x000E:
            /* SHL */
            V[0xF] = V[(op & 0x0F00) >> 8] & 0x8000;
            V[(op * 0x0F00) >> 8] <<= 1;
        break;
        }
    break;
    } /* End 0x8000 */
    case 0x9000:
        /* SKRNE */
        if (V[(op & 0x0F00) >> 8] != V[(op & 0x00F0) >> 4]) {
            pc += 2;
        }
    break;
    case 0xA000:
        /* ILOAD */
        index = op & 0x0FFF;
    break;
    case 0xB000:
        /* ZJMP */
        pc  = (op & 0x0FFF) += V[0];
    break;
    case 0xC000:
        /* RAND */
        V[(op & 0x0F00) >> 8] = (op & 0x00FF) + 0; // TODO: implement random #
    break;
    case 0xD000:
        /* DRAW */
    break;
    case 0xE000: {
        uint16_t low = op & 0x00FF;
        switch (low) {
        case 0x009E:
            /* SKK */
            if (key[(op & 0x0F00) >> 8]) {
                pc += 2;
            }
        break;
        case 0x00A1:
            /* SKNK */
            if (!key[(op & 0x0F00) >> 8]) {
                pc += 2;
            }
        break;
        }
    break;
    } /* End 0xE000 */
    case 0xF000: {
        uint16_t low = op & 0x00FF;
        switch (low) {

        case 0x0007:
            /* DELA */
            V[(op & 0x0F00) >> 8] = delay_timer;
        break;
        case 0x000A:
            /* KEYW */
        break;
        case 0x0015:
            /* DELR */
            delay_timer = V[(op & 0x0F00) >> 8];
        break;
        case 0x0018:
            /* SNDR */
            sound_timer = V[(op & 0x0F00) >> 8];
        break;
        case 0x001E:
            /* IADD */
            index += V[(op & 0x0F00) >> 8];
        break;
        case 0x0029:
            /* SILS */
            // need fontset
        break;
        case 0x0033:
            /* BCD -- Store "102" as "1", "0", "2" in memory */
            memory[index] = V[(op & 0x0F00) >> 8] / 100;
            memory[index + 1] = (V[(op & 0x0F00) >> 8] / 10) % 10;
            memory[index + 2] = (V[(op & 0x0F00) >> 8] % 100) % 10;
        break;
        case 0x0055:
            /* DUMP */
            for (int i = 0; i <= (op & 0x0F00 >> 8); ++i) {
                memory[I + i] = V[i];
            }
        break;
        case 0x0065:
            /* IDUMP */
            for (int i = 0; i <= (op & 0x0F00 >> 8); ++i) {
                V[i] = memory[I + i];
            }
        break;
        }
    break;
    } /* End 0xF000 */
    default:
        // handle error
    break;
    }
}

uint16_t CPU::next()
{
    return memory[pc] << 8 | memory[pc + 1];
}