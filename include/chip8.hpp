#include <stdio.h>
#include <iostream>

class Chip8
{
public:
    bool drawFlag;

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
    unsigned char memory[4096];     // 4K memory
    unsigned short opcode;          // 2 bytes each
    unsigned char cpu_register[16]; // CPU registers - 15 general purpose - 1 carry flag
    unsigned short index_register;  // Index register - 0x000 to 0xFFF
    unsigned short pc;              // Program counter - 0x000 to 0xFFF
    unsigned short stack[16];       // store pc in stack before jump
    unsigned short sp;              // stack pointer
    unsigned char gfx[64 * 32];     // 2048 pixels
    unsigned char key[16];          // 0x0 to 0xF keypad

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short fetch();
    void execute();
    void update_timer();
};