#include <stdio.h>
#include <iostream>
#include <random>
#include <cstdint>

class Chip8
{
public:
    bool draw_flag;

    Chip8();
    void emulate_cycle();
    void set_keys();
    void load_game()
    {
        FILE *file = fopen("../games/Airplane.ch8", "rb");

        // for(int i = 0; i < bufferSize; ++i)
        //     memory[i + 512] = buffer[i]; // 0x200 = 512
    };

private:
    uint8_t memory[4096]; // 4K memory
    uint16_t opcode;      // 2 bytes each
    uint8_t V[16];        // CPU registers - 15 general purpose - 1 carry flag
    uint16_t I;           // Index register - 0x000 to 0xFFF
    uint16_t pc;          // Program counter - 0x000 to 0xFFF
    uint16_t stack[16];   // store pc in stack before jump
    uint16_t sp;          // stack pointer
    uint8_t gfx[64 * 32]; // 2048 pixels
    uint8_t key[16];      // 0x0 to 0xF keypad

    uint8_t delay_timer;
    uint8_t sound_timer;

    std::mt19937 rng;
    std::uniform_int_distribution<int> dist{0, 255};

    uint16_t fetch();
    void execute();
    void update_timer();

    void opSYS();        // 0NNN: Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
    void opCLS();        // 00E0: Clears the screen.
    void opRET();        // 00EE: Returns from a subroutine.
    void opJP();         // 1NNN: Jumps to address NNN.
    void opCALL();       // 2NNN: Calls subroutine at NNN.
    void opSE_VxByte();  // 3XNN: Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block).
    void opSNE_VxByte(); // 4XNN: Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block).
    void opSE_VxVy();    // 5XY0: Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block).
    void opLD_VxByte();  // 6XNN: Sets VX to NN.
    void opADD_VxByte(); // 7XNN: Adds NN to VX (carry flag is not changed).
    void opLD_VxVy();    // 8XY0: Sets VX to the value of VY.
    void opOR_VxVy();    // 8XY1: Sets VX to VX or VY. (bitwise OR operation).
    void opAND_VxVy();   // 8XY2: Sets VX to VX and VY. (bitwise AND operation).
    void opXOR_VxVy();   // 8XY3: Sets VX to VX xor VY.
    void opADD_VxVy();   // 8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
    void opSUB_VxVy();   // 8XY5: VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not.
    void opSHR_Vx();     // 8XY6: Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
    void opSUBN_VxVy();  // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not.
    void opSHL_Vx();     // 8XYE: Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
    void opSNE_VxVy();   // 9XY0: Skips the next instruction if VX does not equal VY.
    void opLD_I();       // ANNN: Sets I to the address NNN.
    void opJP_V0();      // BNNN: Jumps to the address NNN plus V0.
    void opRND();        // CXNN: Sets VX to the result of a bitwise and operation on a random number and NN.
    void opDRW();        // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
    void opSKP_Vx();     // EX9E: Skips the next instruction if the key stored in VX is pressed.
    void opSKNP_Vx();    // EXA1: Skips the next instruction if the key stored in VX is not pressed.
    void opLD_VxDT();    // FX07: Sets VX to the value of the delay timer.
    void opLD_VxK();     // FX0A: A key press is awaited, and then stored in VX (blocking operation).
    void opLD_DTVx();    // FX15: Sets the delay timer to VX.
    void opLD_STVx();    // FX18: Sets the sound timer to VX.
    void opADD_IVx();    // FX1E: Adds VX to I. VF is not affected.
    void opLD_FVx();     // FX29: Sets I to the location of the sprite for the character in VX.
    void opLD_BVx();     // FX33: Stores the binary-coded decimal representation of VX, hundreds at I, tens at I+1, ones at I+2.
    void opLD_IVx();     // FX55: Stores from V0 to VX in memory, starting at address I.
    void opLD_VxI();     // FX65: Fills from V0 to VX with values from memory, starting at address I.

    void opUnknown();
};