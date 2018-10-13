#include "cpu.h"
#include <cstring>
#include <string>
#include <iostream>

CPU::CPU(std::FILE *rom) : sp(0), opcode(0), index(0),
    pc(0x200), delay_timer(0), sound_timer(0), need_draw(false)
{
    /* Load font into memory */
    std::memcpy(memory, chip8_fontset, sizeof(chip8_fontset));

    /* Load game into memory */
    std::fread(memory + CHIP8_START_ADDRESS, sizeof(*memory),
        CHIP8_MEMORY_SIZE - CHIP8_START_ADDRESS, rom);

    /* Clear all registers, stack, graphics */
    std::memset(V, 0, sizeof(V));
    std::memset(gfx, 0, sizeof(gfx));
    std::memset(stack, 0, sizeof(stack));
    std::memset(key, 0, sizeof(key));
}

void CPU::dump()
{
    for (int i = 0; i < CHIP8_REGISTER_COUNT; ++i) {
        LOG("V[" << i << " : " << from_hex(V[i]) << "]");
    }

    for (int i = 0; i < CHIP8_KEY_COUNT; ++i) {
        LOG("K[" << i << " : " << from_hex(key[i]) << "]");
    }
}

void CPU::emulate_cycle()
{
    opcode = next();
    LOG("Opcode: " << from_hex(opcode) << " PC: " << from_hex(pc) << " SP: " << from_hex(sp));
    LOG("Index: " << from_hex(index));
    decode(opcode);
    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        if (sound_timer == 1) {
            std::cout << "BEEP!\n";
        }
        --sound_timer;
    }
    dump();
}

void CPU::decode(uint16_t op)
{
    uint16_t high = op & 0xF000;
    switch (high) {
    case 0x0000: {
        uint16_t low = op & 0x0FFF;
        switch (low) {
        case 0x00E0:
            /* CLR */
            memset(gfx, 0, sizeof(gfx));
            pc += 2;
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
        LOG("Jumping to " << from_hex((op & 0x0FFF)));
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
            pc += 4;
        } else {
            pc += 2;
        }
    break;
    case 0x4000:
        /* SKNE */
        if (V[(op & 0x0F00) >> 8] != (op & 0x00FF)) {
            pc += 4;
        } else {
            pc += 2;
        }
    break;
    case 0x5000:
        /* SKRE */
        if (V[(op & 0x0F00) >> 8] == V[(op & 0x00F0) >> 4]) {
            pc += 4;
        } else {
            pc += 2;
        }
    break;
    case 0x6000:
        /* LOAD */
        V[(op & 0x0F00) >> 8] = op & 0x00FF;
        pc += 2;
    break;
    case 0x7000:
        /* ADD */
        V[(op & 0x0F00) >> 8] += op & 0x00FF;
        pc += 2;
    break;
    case 0x8000: {
        uint16_t low = op & 0x000F;
        switch (low) {
        case 0x0000:
            /* ASN */
            V[(op & 0x00F0) >> 4] = V[(op & 0x0F00) >> 8];
            pc += 2;
        break;
        case 0x0001:
            /* OR */
            V[(op & 0x0F00) >> 8] |= V[(op & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0002:
            /* AND */
            V[(op & 0x0F00) >> 8] &= V[(op & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0003:
            /* XOR */
            V[(op & 0x0F00) >> 8] ^= V[(op & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0004:
            /* RADD */
            if(V[(op & 0x00F0) >> 4] > (0xFF - V[(op & 0x0F00) >> 8]))
                V[0xF] = 1; /* carry */
            else
                V[0xF] = 0;
            V[(op & 0x0F00) >> 8] += V[(op & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0005:
            /* SUB */
            if(V[(op & 0x00F0) >> 4] > (0xFF - V[(op & 0x0F00) >> 8]))
                V[0xF] = 1; /* borrow */
            else
                V[0xF] = 0;
            V[(op & 0x0F00) >> 8] -= V[(op & 0x00F0) >> 4];
            pc += 2;
        break;
        case 0x0006:
            /* SHR */
            V[0xF] = V[(op & 0x0F00) >> 8] & 0x1;
            V[(op * 0x0F00) >> 8] >>= 1;
            pc += 2;
        break;
        case 0x0007:
            /* RSUB */
            V[(op & 0x0F00) >> 8] = V[(op & 0x00F0) >> 4] - V[(op & 0x0F00) >> 8];
            pc += 2;
        break;
        case 0x000E:
            /* SHL */
            V[0xF] = V[(op & 0x0F00) >> 8] & 0x8000;
            V[(op * 0x0F00) >> 8] <<= 1;
            pc += 2;
        break;
        }
    break;
    } /* End 0x8000 */
    case 0x9000:
        /* SKRNE */
        if (V[(op & 0x0F00) >> 8] != V[(op & 0x00F0) >> 4]) {
            pc += 4;
        } else {
            pc += 2;
        }
    break;
    case 0xA000:
        /* ILOAD */
        index = op & 0x0FFF;
        pc += 2;
    break;
    case 0xB000:
        /* ZJMP */
        pc  = (op & 0x0FFF) + V[0];
    break;
    case 0xC000:
        /* RAND */
        V[(op & 0x0F00) >> 8] = (op & 0x00FF) + std::rand();
        pc += 2;
    break;
    case 0xD000: {
        /* DRAW */
        uint8_t x = V[(op & 0x0F00) >> 8];
        uint8_t y = V[(op & 0x00F0) >> 4];
        uint8_t height = op & 0x000F;

        V[0xF] = 0; /* Clear the collision bit */
        for (uint8_t h = 0; h < height; ++h) {
            const uint8_t pixel = memory[index + h];
            /* Now read in the row value */
            for (uint8_t w = 0; w < 8; ++w) {
                /* Check if the bit is set in the 8-pixel row */
                if (pixel & (0x0080 >> w)) {
                    const uint8_t offset = x + w + ((y + h) * CHIP8_PIXELS_WIDTH);
                    if (gfx[offset] == 1) {
                        V[0xF] = 1; /* Collision! */
                    }
                    gfx[offset] ^= 1;
                }
            }
        }
        need_draw = true;
        pc += 2;
    }
    break;
    case 0xE000: {
        uint16_t low = op & 0x00FF;
        switch (low) {
        case 0x009E:
            /* SKK */
            if (key[(op & 0x0F00) >> 8]) {
                pc += 4;
            } else {
                pc += 2;
            }
        break;
        case 0x00A1:
            /* SKNK */
            if (!key[(op & 0x0F00) >> 8]) {
                pc += 4;
            } else {
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
            pc += 2;
        break;
        case 0x000A: {
            /* KEYW */
            const uint8_t *keys = SDL_GetKeyboardState(nullptr);
            for (uint8_t i = 0; i < CHIP8_KEY_COUNT; ++i) {
                if (keys[keymap[i]]) {
                    V[(op & 0x0F00 >> 8)] = i;
                    pc += 2;
                }
            }
        }
        break;
        case 0x0015:
            /* DELR */
            delay_timer = V[(op & 0x0F00) >> 8];
            pc += 2;
        break;
        case 0x0018:
            /* SNDR */
            sound_timer = V[(op & 0x0F00) >> 8];
            pc += 2;
        break;
        case 0x001E:
            /* IADD */
            index += V[(op & 0x0F00) >> 8];
            pc += 2;
        break;
        case 0x0029:
            /* SILS */
            index = V[(op & 0x0F00) >> 8] * 5;
            pc += 2;
        break;
        case 0x0033:
            /* BCD -- Store "102" as "1", "0", "2" in memory */
            memory[index] = V[(op & 0x0F00) >> 8] / 100;
            memory[index + 1] = (V[(op & 0x0F00) >> 8] / 10) % 10;
            memory[index + 2] = (V[(op & 0x0F00) >> 8] % 100) % 10;
            pc += 2;
        break;
        case 0x0055:
            /* DUMP */
            for (int i = 0; i <= (op & 0x0F00 >> 8); ++i) {
                memory[index + i] = V[i];
            }
            pc += 2;
        break;
        case 0x0065:
            /* IDUMP */
            for (int i = 0; i <= (op & 0x0F00 >> 8); ++i) {
                V[i] = memory[index + i];
            }
            pc += 2;
        break;
        }
    break;
    } /* End 0xF000 */
    default:
        LOG(op << " is an invalid opcode!");
    break;
    }
}

uint16_t CPU::next()
{
    return memory[pc] << 8 | memory[pc + 1];
}