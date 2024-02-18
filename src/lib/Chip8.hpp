//
// Created by piper on 2/17/24.
//

#define SHIT_8_CHIP8_HPP

#include <cstdint>
#include <random>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;


class Chip8 {
public:
    Chip8();

    uint32_t video[64 * 32]{};
    uint8_t keypad[16]{};

    void LoadRom(const char *filename);

    void Cycle();

private:
    void Table0();

    void Table8();

    void TableE();

    void TableF();

    // Do nothing
    void OP_NULL();

    // CLS (clear screen)
    void OP_00E0();

    // RET (return)
    void OP_00EE();

    // JP address (jump to address)
    void OP_1nnn();

    // CALL address
    void OP_2nnn();

    // SE Vx, byte (skip next instruction if Vx (register x) equals kk)
    void OP_3xkk();

    // SNE Vx, byte (skip next instruction if Vx is not equal to kk)
    void OP_4xkk();

    // SE Vx, Vy (skip next instruction if Vx equals Vy)
    void OP_5xy0();

    // LD Vx, byte (Set Vx to be kk)
    void OP_6xkk();

    // ADD Vx, byte (Set Vx to be Vx + kk)
    void OP_7xkk();

    // LD Vx, Vy (Set Vx to be Vy)
    void OP_8xy0();

    // OR Vx, Vy (Do the OR boolean operation on Vx and Vy)
    void OP_8xy1();

    // AND Vx, Vy
    void OP_8xy2();

    // XOR Vx, Vy
    void OP_8xy3();

    // ADD Vx, Vy (Set Vx to be Vx + Vy, VF is then set to be the carry)
    void OP_8xy4();

    // SUB Vx, Vy (Set Vx to be Vx, Vy, Vf is set to be NOT borrow)
    // If Vx > Vy, VF is then set to 1, otherwise 0
    void OP_8xy5();

    // SHR Vx (Set Vx = Vx SHR (wut?) 1)
    // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
    // Then Vx is divided by 2 (oh)
    void OP_8xy6();

    // SUBN Vx, Vy (Set Vx to be Vy - Vx, set VF = NOT borrow)
    // If Vy > Vx, then VF is set to 1, otherwise 0
    void OP_8xy7();

    // SHL Vx {, Vy} (Set Vx = Vx SHL (I should really read up on what these mean) 1)
    // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise 0
    // Then Vx is multiplied by 2
    void OP_8xyE();

    // SNE Vx, Vy (Skip next instruction if Vx != Vy)
    void OP_9xy0();

    // LD I, address (Set I (index register) = nnn)
    void OP_Annn();

    // JP V0, address (Jump to location nnn + V0)
    void OP_Bnnn();

    // RND Vx, byte (Set Vx = random byte AND kk)
    void OP_Cxkk();

    // Here's a fun one :)
    // DRW Vx, Vy, nibble (Display n-byte sprite starting at memory location
    // I at (Vx, Vy), set VF = collision
    void OP_Dxyn();

    // SKP Vx (Skip next instruction if key with the value Vx is pressed)
    void OP_Ex9E();

    // SKNP Vx (Skip next instruction if key Vx is not pressed
    void OP_ExA1();

    // LD Vx, DT (Set Vx to be the delay timer value)
    void OP_Fx07();

    // LD Vx, K (Wait for a keypress, store the value of the key in Vx)
    void OP_Fx0A();

    // LD DT, Vx (Set delay timer to Vx)
    void OP_Fx15();

    // LD ST, Vx (Set sound timer to Vx)
    void OP_Fx18();

    // ADD I, Vx (Set I to I + Vx)
    void OP_Fx1E();

    // LD F, Vx (Set I to location of spite for digit Vx)
    void OP_Fx29();

    // LD B, Vx (Store BCD representation of Vx in memory locations
    // I, I+1 and I+2
    // The interpreter takes the decimal value of Vx and places
    // the hundreds digit in memory at location I,
    // the tens digit at location I+1,
    // and the ones digit at location I+2
    void OP_Fx33();

    // LD [I], Vx (Store registers v0 through Vx in memory starting at location I)
    void OP_Fx55();

    // LD Vx, [I] (Read registers v0 through Vx from memory starting at location I)
    void OP_Fx65();

    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index{};
    uint16_t pc{}; // Program counter
    uint16_t stack[16]{};
    uint8_t sp{}; // Stack pointer
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint16_t opcode{};

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    typedef void (Chip8::*Chip8Func)();

    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];
};
