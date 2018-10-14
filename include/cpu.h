#pragma once

#include <cstdio>
#include <SDL2/SDL.h>
#include "common.h"

static const unsigned char chip8_fontset[CHIP8_FONT_COUNT] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

static const int keymap[CHIP8_KEY_COUNT] = {
    SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,
    SDLK_a,
    SDLK_b,
    SDLK_c,
    SDLK_d,
    SDLK_e,
    SDLK_f
};

class CPU {
public:
    CPU(ROM rom);

    void emulate_cycle();
    uint16_t next();
    void decode(uint16_t op);

    void dump();
    bool needsDraw() const;

    void setDraw(bool draw);

    uint8_t* getGFX();

private:
    uint8_t memory[CHIP8_MEMORY_SIZE]; /* Available memory */

    uint8_t gfx[CHIP8_PIXELS_WIDTH * CHIP8_PIXELS_HEIGHT]; /* Graphics memory */

    uint8_t key[CHIP8_KEY_COUNT]; /* HEX-based keypad (0x0 - 0xF) */
    uint8_t V[CHIP8_REGISTER_COUNT]; /* Registers 0-15 and carry */

    /* Stack and stack pointer for CALL routines */
    uint16_t stack[CHIP8_STACK_DEPTH];
    uint16_t sp;

    uint16_t opcode; /* The current opcode we're processing */
    uint16_t index; /* Index register */
    uint16_t pc; /* Program counter */

    /*
     * Timer registers that count down to zero if > 0.
     * Once time reaches 0, a beep occurs.
     */
    uint8_t delay_timer;
    uint8_t sound_timer;

    bool need_draw;

    void processF(uint16_t op);
    void processE(uint16_t op);
    void processD(uint16_t op);
    void processC(uint16_t op);
    void processB(uint16_t op);
    void processA(uint16_t op);
    void process9(uint16_t op);
    void process8(uint16_t op);
    void process7(uint16_t op);
    void process6(uint16_t op);
    void process5(uint16_t op);
    void process4(uint16_t op);
    void process3(uint16_t op);
    void process2(uint16_t op);
    void process1(uint16_t op);
    void process0(uint16_t op);
};