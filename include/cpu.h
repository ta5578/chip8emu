#pragma once

#include <cstdio>
#include <SDL2/SDL.h>
#include "common.h"

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