#include "chip8.hpp"
#include <fstream>

#define RANDOM_NUMBER 12;

uint8_t chip8_fontset[80] =
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

Chip8::Chip8() : pc{0x200}, opcode{0}, I{0}, sp{0}, rng(std::random_device{}())
{
  // Clear display
  // Clear stack
  // Clear registers V0-VF
  // Clear memory

  // Load fontset
  for (int i = 0; i < 80; ++i) // 0x50
    memory[i] = chip8_fontset[i];

  // Reset timers
}

void Chip8::emulate_cycle()
{
  fetch();
  execute();

  update_timer();
}

void Chip8::update_timer()
{
  if (delay_timer > 0)
  {
    --delay_timer;
  }
  if (sound_timer > 0)
  {
    if (sound_timer == 1)
      std::cout << "BEEP!\n";
    --sound_timer;
  }
}

uint16_t Chip8::fetch()
{
  return memory[pc] << 8 | memory[pc + 1];
}

void Chip8::execute()
{
  switch (opcode & 0xF000)
  {
  case 0x0000:
    switch (opcode & 0x00FF)
    {
    case 0x00E0:
      opCLS();
      break;
    case 0x00EE:
      opRET();
      break;
    default:
      opSYS();
      break;
    }
    break;

  case 0x1000:
    opJP();
    break;
  case 0x2000:
    opCALL();
    break;
  case 0x3000:
    opSE_VxByte();
    break;
  case 0x4000:
    opSNE_VxByte();
    break;
  case 0x5000:
    opSE_VxVy();
    break;
  case 0x6000:
    opLD_VxByte();
    break;
  case 0x7000:
    opADD_VxByte();
    break;

  case 0x8000:
    switch (opcode & 0x000F)
    {
    case 0x0000:
      opLD_VxVy();
      break;
    case 0x0001:
      opOR_VxVy();
      break;
    case 0x0002:
      opAND_VxVy();
      break;
    case 0x0003:
      opXOR_VxVy();
      break;
    case 0x0004:
      opADD_VxVy();
      break;
    case 0x0005:
      opSUB_VxVy();
      break;
    case 0x0006:
      opSHR_Vx();
      break;
    case 0x0007:
      opSUBN_VxVy();
      break;
    case 0x000E:
      opSHL_Vx();
      break;
    default:
      opUnknown();
      break;
    }
    break;

  case 0x9000:
    opSNE_VxVy();
    break;
  case 0xA000:
    opLD_I();
    break;
  case 0xB000:
    opJP_V0();
    break;
  case 0xC000:
    opRND();
    break;
  case 0xD000:
    opDRW();
    break;

  case 0xE000:
    switch (opcode & 0x00FF)
    {
    case 0x009E:
      opSKP_Vx();
      break;
    case 0x00A1:
      opSKNP_Vx();
      break;
    default:
      opUnknown();
      break;
    }
    break;

  case 0xF000:
    switch (opcode & 0x00FF)
    {
    case 0x0007:
      opLD_VxDT();
      break;
    case 0x000A:
      opLD_VxK();
      break;
    case 0x0015:
      opLD_DTVx();
      break;
    case 0x0018:
      opLD_STVx();
      break;
    case 0x001E:
      opADD_IVx();
      break;
    case 0x0029:
      opLD_FVx();
      break;
    case 0x0033:
      opLD_BVx();
      break;
    case 0x0055:
      opLD_IVx();
      break;
    case 0x0065:
      opLD_VxI();
      break;
    default:
      opUnknown();
      break;
    }
    break;

  default:
    opUnknown();
    break;
  }
}

/**
 * @brief 0NNN: Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
 */
void Chip8::opSYS() {}

/**
 * @brief 00E0: Clears the screen.
 */
void Chip8::opCLS()
{
  for (int i = 0; i < 64 * 32; i++)
    gfx[i] = 0;
  pc += 2;
}

/**
 * @brief 00EE: Returns from a subroutine.
 */
void Chip8::opRET()
{
  pc = stack[--sp];
}

/**
 * @brief 1NNN: Jumps to address NNN.
 */
void Chip8::opJP()
{
  pc = opcode & 0x0FFF;
}

/**
 * @brief 2NNN: Calls subroutine at NNN.
 */
void Chip8::opCALL()
{
  stack[sp] = pc;
  ++sp;
  pc = opcode & 0x0FFF;
}

/**
 * @brief 3XNN: Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block).
 */
void Chip8::opSE_VxByte()
{
  if (V[(opcode & 0x0F00) >> 8] == opcode & 0x00FF)
    pc += 4;
  else
    pc += 2;
}

/**
 * @brief 4XNN: Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block).
 */
void Chip8::opSNE_VxByte()
{
  if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
    pc += 4;
  else
    pc += 2;
}

/**
 * @brief 5XY0: Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block).
 */
void Chip8::opSE_VxVy()
{
  if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
    pc += 4;
  else
    pc += 2;
}

/**
 * @brief 6XNN: Sets VX to NN.
 */
void Chip8::opLD_VxByte()
{
  V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
  pc += 2;
}

/**
 * @brief 7XNN: Adds NN to VX (carry flag is not changed).
 */
void Chip8::opADD_VxByte()
{
  V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
  pc += 2;
}

/**
 * @brief 8XY0: Sets VX to the value of VY.
 */
void Chip8::opLD_VxVy()
{
  V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

/**
 * @brief 8XY1: Sets VX to VX or VY. (bitwise OR operation).
 */
void Chip8::opOR_VxVy()
{
  V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

/**
 * @brief 8XY2: Sets VX to VX and VY. (bitwise AND operation).
 */
void Chip8::opAND_VxVy()
{
  V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

/**
 * @brief 8XY3: Sets VX to VX xor VY.
 */
void Chip8::opXOR_VxVy()
{
  V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
  pc += 2;
}

/**
 * @brief 8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
 */
void Chip8::opADD_VxVy()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  if (V[y] > (0xFF - V[x]))
    V[0xF] = 1; // carry
  else
    V[0xF] = 0;
  V[x] += V[y];
  pc += 2;
}

/**
 * @brief 8XY5: VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
 */
void Chip8::opSUB_VxVy()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  if (V[y] <= V[x])
    V[0xF] = 1; // carry
  else
    V[0xF] = 0;
  V[x] -= V[y];
  pc += 2;
}

/**
 * @brief 8XY6: Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
 */
void Chip8::opSHR_Vx()
{
  uint8_t x = (opcode & 0x0F00) >> 8;

  V[0xF] = V[x] & 0x1;
  V[x] >>= 1;
  pc += 2;
}

/**
 * @brief 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
 */
void Chip8::opSUBN_VxVy()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  if (V[y] >= V[x])
    V[0xF] = 1; // carry
  else
    V[0xF] = 0;
  V[x] = V[y] - V[x];
  pc += 2;
}

/**
 * @brief 8XYE: Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
 */
void Chip8::opSHL_Vx()
{
  uint8_t x = (opcode & 0x0F00) >> 8;

  V[0xF] = V[x] & 0xF;
  V[x] <<= 1;
  pc += 2;
}

/**
 * @brief 9XY0: Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block).
 */
void Chip8::opSNE_VxVy()
{
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  if (V[y] != V[x])
    pc += 4;
  else
    pc += 2;
}

/**
 * @brief ANNN: Sets I to the address NNN.
 */
void Chip8::opLD_I()
{
  I = opcode & 0x0FFF;
  pc += 2;
}

/**
 * @brief BNNN: Jumps to the address NNN plus V0.
 */
void Chip8::opJP_V0()
{
  pc = (opcode & 0x0FFF) + V[0];
}

/**
 * @brief CXNN: Sets VX to the result of a bitwise and operation on a random number (typically 0 to 255) and NN.
 */
void Chip8::opRND()
{
  uint8_t random_number = static_cast<uint8_t>(dist(rng));
  V[(opcode & 0x0F00) >> 8] = random_number & (opcode & 0x00FF);
  pc += 2;
}

/**
 * @brief DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
 */
void Chip8::opDRW() {}

/**
 * @brief EX9E: Skips the next instruction if the key stored in VX (only consider the lowest nibble) is pressed (usually the next instruction is a jump to skip a code block).
 */
void Chip8::opSKP_Vx() {}

/**
 * @brief EXA1: Skips the next instruction if the key stored in VX (only consider the lowest nibble) is not pressed (usually the next instruction is a jump to skip a code block).
 */
void Chip8::opSKNP_Vx() {}

/**
 * @brief FX07: Sets VX to the value of the delay timer.
 */
void Chip8::opLD_VxDT() {}

/**
 * @brief FX0A: A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing).
 */
void Chip8::opLD_VxK() {}

/**
 * @brief FX15: Sets the delay timer to VX.
 */
void Chip8::opLD_DTVx()
{
  delay_timer = V[(opcode & 0x0F00) >> 8];
  pc += 2;
}

/**
 * @brief FX18: Sets the sound timer to VX.
 */
void Chip8::opLD_STVx()
{
  sound_timer = V[(opcode & 0x0F00) >> 8];
  pc += 2;
}

/**
 * @brief FX1E: Adds VX to I. VF is not affected.
 */
void Chip8::opADD_IVx()
{
  I += V[(opcode & 0x0F00) >> 8];
  pc += 2;
}

/**
 * @brief FX29: Sets I to the location of the sprite for the character in VX (only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font.
 */
void Chip8::opLD_FVx() {}

/**
 * @brief FX33: Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
 */
void Chip8::opLD_BVx()
{
  memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
  memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
  memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
  pc += 2;
}

/**
 * @brief FX55: Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
 */
void Chip8::opLD_IVx() {}

/**
 * @brief FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.
 */
void Chip8::opLD_VxI()
{
}

/**
 * @brief Handles an unknown/unimplemented opcode.
 */
void Chip8::opUnknown() {}