#include "cpu.h"
#include <cstring>
#include <iostream>
#include "utils.h"

CPU::CPU(ROM rom) : sp(0), opcode(0), index(0),
    pc(CHIP8_START_ADDRESS), delay_timer(0), sound_timer(0), need_draw(false)
{
    /* Clear all registers, stack, keys, graphics */
    std::memset(V, 0, sizeof(V));
    std::memset(gfx, 0, sizeof(gfx));
    std::memset(stack, 0, sizeof(stack));
    std::memset(key, 0, sizeof(key));

    /* Clear memory */
    std::memset(memory, 0, sizeof(memory));

    /* Load font into memory */
    std::memcpy(memory, chip8_fontset, sizeof(chip8_fontset));

    /* Load game into memory */
    std::memcpy(memory + CHIP8_START_ADDRESS, rom.get(), CHIP8_MEMORY_SIZE - CHIP8_START_ADDRESS);
}

void CPU::dump()
{
    for (int i = 0; i < CHIP8_REGISTER_COUNT; ++i) {
        LOG("V[%d] : 0x%02X", i, V[i]);
    }

    for (int i = 0; i < CHIP8_KEY_COUNT; ++i) {
        LOG("K[%d] : 0x%02X", i, key[i]);
    }
}

void CPU::emulate_cycle()
{
    opcode = next();
    LOG("Fetched 0x%04X", opcode);
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
}

void CPU::decode(uint16_t op)
{
    const uint16_t high = op & 0xF000;
    switch (high) {
    case 0x0000:
        process0(op);
        break;
    case 0x1000:
        process1(op);
        break;
    case 0x2000:
        process2(op);
        break;
    case 0x3000:
        process3(op);
        break;
    case 0x4000:
        process4(op);
        break;
    case 0x5000:
        process5(op);
        break;
    case 0x6000:
        process6(op);
        break;
    case 0x7000:
        process7(op);
        break;
    case 0x8000:
        process8(op);
        break;
    case 0x9000:
        process9(op);
        break;
    case 0xA000:
        processA(op);
        break;
    case 0xB000:
        processB(op);
        break;
    case 0xC000:
        processC(op);
        break;
    case 0xD000:
        processD(op);
        break;
    case 0xE000:
        processE(op);
        break;
    case 0xF000:
        processF(op);
        break;
    default:
        LOG("%d is an invalid opcode!", op);
        break;
    }
}

void CPU::processF(uint16_t op)
{
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint16_t low = op & 0x00FF;
    switch (low) {
    case 0x0007:
        /* DELA */
        V[X] = delay_timer;
        pc += 2;
        break;
    case 0x000A:
    {
        /* KEYW */
        const uint8_t *keys = SDL_GetKeyboardState(nullptr);
        for (uint8_t i = 0; i < CHIP8_KEY_COUNT; ++i) {
            if (keys[keymap[i]]) {
                V[X] = i;
                pc += 2;
            }
        }
    }
    break;
    case 0x0015:
        /* DELR */
        delay_timer = V[X];
        pc += 2;
        break;
    case 0x0018:
        /* SNDR */
        sound_timer = V[X];
        pc += 2;
        break;
    case 0x001E:
        /* IADD */
        index += V[X];
        pc += 2;
        break;
    case 0x0029:
        /* SILS */
        index = V[X] * 5;
        pc += 2;
        break;
    case 0x0033:
        /* BCD -- Store "102" as "1", "0", "2" in memory */
        memory[index] = V[X] / 100;
        memory[index + 1] = (V[X] / 10) % 10;
        memory[index + 2] = (V[X] % 100) % 10;
        pc += 2;
        break;
    case 0x0055:
        /* DUMP */
        for (int i = 0; i <= X; ++i) {
            memory[index + i] = V[i];
        }
        pc += 2;
        break;
    case 0x0065:
        /* IDUMP */
        for (int i = 0; i <= X; ++i) {
            V[i] = memory[index + i];
        }
        pc += 2;
        break;
    }
}

void CPU::processE(uint16_t op)
{
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint16_t low = op & 0x00FF;
    switch (low) {
    case 0x009E:
        /* SKK */
        if (key[X]) {
            pc += 4;
        } else {
            pc += 2;
        }
        break;
    case 0x00A1:
        /* SKNK */
        if (!key[X]) {
            pc += 4;
        } else {
            pc += 2;
        }
        break;
    default:
        LOG("processE() 0x%04X isn't recognized.", op);
        break;
    }
}

void CPU::processD(uint16_t op)
{
    /* DRAW */
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t Y = (op & 0x00F0) >> 4;

    const uint8_t col = V[X];
    const uint8_t row = V[Y];
    const uint8_t height = op & 0x000F;

    V[0xF] = 0; /* Clear the collision bit */
    for (uint8_t h = 0; h < height; ++h) {
        /* Now read in a row of 8 sprite pixels */
        const uint8_t spriteRow = memory[index + h];
        for (uint8_t b = 0; b < 8; ++b) {
            /* Check if the bit is set in the sprite */
            if (spriteRow & (0x0080 >> b)) {
                const uint8_t offset = col + b + ((row + h) * CHIP8_PIXELS_WIDTH);
                if (gfx[offset] == 1) {
                    V[0xF] = 1; /* Collision! This happens since XOR'ing 1 and 1 will result in 0. */
                }
                gfx[offset] ^= 1;
            }
        }
    }
    need_draw = true;
    pc += 2;
}

void CPU::processC(uint16_t op)
{
    /* RAND */
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t NN = (op & 0x00FF);
    V[X] = NN & static_cast<uint8_t>(std::rand());
    pc += 2;
}

void CPU::processB(uint16_t op)
{
    /* ZJMP */
    pc = (op & 0x0FFF) + V[0];
}

void CPU::processA(uint16_t op)
{
    /* ILOAD */
    index = op & 0x0FFF;
    pc += 2;
}

void CPU::process9(uint16_t op)
{
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t Y = (op & 0x00F0) >> 4;
    /* SKRNE */
    if (V[X] != V[Y]) {
        pc += 4;
    } else {
        pc += 2;
    }
}

void CPU::process8(uint16_t op)
{
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t Y = (op & 0x00F0) >> 4;
    const uint16_t low = op & 0x000F;
    switch (low) {
    case 0x0000:
        /* ASN */
        V[X] = V[Y];
        pc += 2;
        break;
    case 0x0001:
        /* OR */
        V[X] |= V[Y];
        pc += 2;
        break;
    case 0x0002:
        /* AND */
        V[X] &= V[Y];
        pc += 2;
        break;
    case 0x0003:
        /* XOR */
        V[X] ^= V[Y];
        pc += 2;
        break;
    case 0x0004:
        /* RADD */
        if (V[Y] > (0xFF - V[X])) // TODO: visit this logic
            V[0xF] = 1; /* carry */
        else
            V[0xF] = 0;
        V[X] += V[Y];
        pc += 2;
        break;
    case 0x0005:
        /* SUB */
        if (V[Y] > (0xFF - V[X])) // TODO: visit this logic
            V[0xF] = 0; /* borrow */
        else
            V[0xF] = 1;
        V[X] -= V[Y];
        pc += 2;
        break;
    case 0x0006:
        /* SHR */
        V[0xF] = V[X] & 0x1;
        V[X] >>= 1;
        pc += 2;
        break;
    case 0x0007:
        /* RSUB */
        if (V[Y] > (0xFF - V[X])) // TODO: visit this logic
            V[0xF] = 0; /* borrow */
        else
            V[0xF] = 1;
        V[X] = V[Y] - V[X];
        pc += 2;
        break;
    case 0x000E:
        /* SHL */
        V[0xF] = V[X] & 0x8000; // TODO: does this store the 1 and 0 as '1' and '0' or as the value?
        V[X] <<= 1;
        pc += 2;
        break;
    default:
        LOG("process8() 0x%04X isn't recognized", op);
        break;
    }
}

void CPU::process7(uint16_t op)
{
    /* ADD */
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t NN = (op & 0x00FF);
    V[X] += NN;
    pc += 2;
}

void CPU::process6(uint16_t op)
{
    /* LOAD */
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t NN = (op & 0x00FF);
    V[X] = NN;
    pc += 2;
}

void CPU::process5(uint16_t op)
{
    /* SKRE */
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t Y = (op & 0x00F0) >> 4;
    if (V[X] == V[Y]) {
        pc += 4;
    } else {
        pc += 2;
    }
}

void CPU::process4(uint16_t op)
{
    /* SKNE */
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t NN = (op & 0x00FF);
    if (V[X] != NN) {
        pc += 4;
    } else {
        pc += 2;
    }
}

void CPU::process3(uint16_t op)
{
    /* SKE */
    const uint8_t X = (op & 0x0F00) >> 8;
    const uint8_t NN = (op & 0x00FF);
    if (V[X] == NN) {
        pc += 4;
    } else {
        pc += 2;
    }
}

void CPU::process2(uint16_t op)
{
    /* CALL */
    stack[sp++] = pc;
    pc = op & 0x0FFF;
}

void CPU::process1(uint16_t op)
{
    /* JMP */
    const uint16_t dest = (op & 0x0FFF);
    LOG("Jumping to 0x%04X", dest);
    pc = dest;
}

void CPU::process0(uint16_t op)
{
    const auto low = op & 0x0FFF;
    switch (low) {
    case 0x00E0:
        /* CLR */
        memset(gfx, 0, sizeof(gfx));
        pc += 2;
        need_draw = true;
        break;
    case 0x00EE:
        /* RET */
        pc = stack[--sp];
        break;
    default:
        LOG("process0() 0x%04X is unrecognized.", op);
    }
}

uint16_t CPU::next()
{
    return memory[pc] << 8 | memory[pc + 1];
}

void CPU::setDraw(bool draw)
{
    need_draw = draw;
}

bool CPU::needsDraw() const
{
    return need_draw;
}

uint8_t* CPU::getGFX()
{
    return &gfx[0];
}