#pragma once

#include "vm_def.h"
#include <memory>

class CPU {

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

    uint16_t next();
    void decode(uint16_t op);

public:
    CPU(std::unique_ptr<uint8_t> rom, size_t len);
    ~CPU();

    void emulate_cycle();

    bool is_draw_set() const;

    void draw();
};