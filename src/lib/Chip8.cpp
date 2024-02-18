//
// Created by Piper8x7b on 2/17/24.
//

#include "Chip8.hpp"
#include <fstream>
#include <chrono>
#include <random>
#include <cstring>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;

uint8_t fontset[FONTSET_SIZE] =
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

Chip8::Chip8()
        : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    // Initialize PC
    pc = START_ADDRESS;

    // Load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // Initialize RNG
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // Set up function pointer table
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++) {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++) {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::Table0() {
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8() {
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE() {
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF() {
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

// Define a method to load a ROM into the Chip8's memory
void Chip8::LoadRom(char const *filename) {
    // Declare file variables
    FILE *pFile;
    long lSize;
    char *buffer;
    size_t result;

    // Open the file in binary read mode
    pFile = std::fopen(filename, "rb");
    // If the file could not be opened, print an error and exit
    if (pFile == NULL) {
        std::fputs("File Error", stderr);
        exit(1);
    }

    // Seek to the end of the file
    std::fseek(pFile, 0, SEEK_END);
    // Get the current file position (which is the size of the file)
    lSize = std::ftell(pFile);
    // Rewind the file position indicator to the beginning of the file
    rewind(pFile);

    // Allocate memory to hold the file data
    buffer = (char *) malloc(sizeof(char) * lSize);
    // If the memory could not be allocated, print an error and exit
    if (buffer == NULL) {
        std::fputs("Memory error", stderr);
        exit(2);
    }

    // Read the file into the buffer
    result = std::fread(buffer, 1, lSize, pFile);
    // If the entire file could not be read, print an error and exit
    if (result != lSize) {
        fputs("Reading error", stderr);
        exit(3);
    }

    // Close the file
    std::fclose(pFile);

    // Copy the file data into the Chip8 system's memory, starting at START_ADDRESS
    for (long i = 0; i < lSize; i++) {
        memory[START_ADDRESS + i] = buffer[i];
    }

    // Free the memory allocated for the buffer
    free(buffer);
}


void Chip8::Cycle() {
    // Fetch
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    // Increment the PC before we execute anything
    pc += 2;

    // Decode and Execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // Decrement the delay timer if it's been set
    if (delayTimer > 0) {
        --delayTimer;
    }

    // Decrement the sound timer if it's been set
    if (soundTimer > 0) {
        --soundTimer;
    }
}


// Do nothing
void Chip8::OP_NULL() {

}

// CLS (clear screen)
void Chip8::OP_00E0() {
    // Set the entire video array to be 0s
    memset(video, 0, sizeof(video));
}

// RET (return)
void Chip8::OP_00EE() {
    --sp; // Decrement the stack pointer
    pc = stack[sp]; // Set the program counter to the value the stack pointer is pointing to
}

// JP address (jump to address)
void Chip8::OP_1nnn() {
    // I *think* this extracts the lower 8 bits of opcode,
    // Thus getting the address input to the command.
    // I had to ask mistral-medium what this meant lol
    uint16_t address = opcode & 0x0FFFu;

    pc = address; // Set the program counter to the new address
}

// CALL address
void Chip8::OP_2nnn() {
    // Get the input to the command
    uint16_t address = opcode & 0x0FFFu;

    // Push the program counter to the stack
    // at the location the stack pointer is pointing to
    stack[sp] = pc;
    sp++; // Move the stack pointer forward
    pc = address; // Finally move the program counter to the new address
}

// SE Vx, byte (skip next instruction if Vx (register x) equals kk)
void Chip8::OP_3xkk() {
    // Intuition tells me this just gets the value of x and kk with weird bit-math
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // The register to test
    uint8_t byte = opcode & 0x00FFu; // The value to test with

    // If the selected register (Vx) is equal to kk skip the next instruction
    if (registers[Vx] == byte) {
        pc += 2;
    }
}

// SNE Vx, byte (skip next instruction if Vx is not equal to kk)
void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // The register to test
    uint8_t byte = opcode & 0x00FFu; // The value to test with

    // If the selected register (Vx) is not equal to kk skip the next instruction
    if (registers[Vx] != byte) {
        pc += 2;
    }
}

// SE Vx, Vy (skip next instruction if Vx equals Vy)
void Chip8::OP_5xy0() {
    // More weird bit-math the tutorial does NOT explain! :D
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // The first register to test
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // The second register to test

    // If the first selected register (Vx) is equal to the second (Vy)
    // skip the next instruction
    if (registers[Vx] == registers[Vy]) {
        pc += 2;
    }
}

// LD Vx, byte (Set Vx to be kk)
void Chip8::OP_6xkk() {
    // I've explained this vit of code enough
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    // Set the register to the desired value
    registers[Vx] = byte;
}

// ADD Vx, byte (Set Vx to be Vx + kk)
void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    // You get the gist
    registers[Vx] += byte;
}

// LD Vx, Vy (Set Vx to be Vy)
void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

// OR Vx, Vy (Do the OR boolean operation on Vx and Vy)
void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
}

// AND Vx, Vy
void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

// XOR Vx, Vy
void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

// ADD Vx, Vy (Set Vx to be Vx + Vy, VF is then set to be the carry)
void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // Get the sum of Vx and Vy
    uint16_t sum = registers[Vx] + registers[Vy];

    // If the sum is greater than 255, set the carry register (VF) to 1
    // Else, set the carry register to 0
    if (sum > 255u) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    // Finally set Vx to be the sum in uint8 form
    // I really wish this tutorial explained the bit-math...
    registers[Vx] = sum & 0xFFu;
}

// SUB Vx, Vy (Set Vx to be Vx, Vy, Vf is set to be NOT borrow)
// If Vx > Vy, VF is then set to 1, otherwise 0
void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // Set VF to be 1 if Vx > Vy, otherwise 0
    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    // Finally do the operation
    registers[Vx] -= registers[Vy];
}

// SHR Vx (Set Vx = Vx SHR (wut?) 1)
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
// Then Vx is divided by 2 (oh)
// MORE BIT-MATH THE TUTORIAL REFUSES TO EXPLAIN!!!!!
void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // Save LSB (huh?) in VF
    registers[0xF] = (registers[Vx] & 0x1u);

    // This divides Vx by 2 ig?
    registers[Vx] >>= 1;
}

// SUBN Vx, Vy (Set Vx to be Vy - Vx, set VF = NOT borrow)
// If Vy > Vx, then VF is set to 1, otherwise 0
void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // Set VF to be 1 if Vy > Vx, otherwise 0
    if (registers[Vy] > registers[Vx]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    // Do the operation
    registers[Vx] = registers[Vy] - registers[Vx];
}

// SHL Vx {, Vy} (Set Vx = Vx SHL (I should really read up on what these mean) 1)
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise 0
// Then Vx is multiplied by 2
void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // Save MSB in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    // I'm thinking this is bit-math for multiplying by 2
    registers[Vx] <<= 1;
}

// SNE Vx, Vy (Skip next instruction if Vx != Vy)
void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // Skip the next instruction if Vx != Vy
    if (registers[Vx] != registers[Vy]) {
        pc += 2;
    }
}

// LD I, address (Set I (index register) = nnn)
void Chip8::OP_Annn() {
    uint16_t address = opcode & 0x0FFFu;

    index = address;
}

// JP V0, address (Jump to location nnn + V0)
void Chip8::OP_Bnnn() {
    uint16_t address = opcode & 0x0FFFu;

    pc = registers[0] + address;
}

// RND Vx, byte (Set Vx = random byte AND kk)
void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;

}

// Here's a fun one :)
// DRW Vx, Vy, nibble (Display n-byte sprite starting at memory location
// I at (Vx, Vy), set VF = collision
void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    // Wrap the sprite if going beyond screen bounds
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    // Init VF to be 0 in case we don't get a collision
    registers[0xF] = 0;

    // Apart from the loops this is all some black magic bs 🥴
    for (unsigned int row = 0; row < height; row++) {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t *screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            // Sprite pixel is on
            if (spritePixel) {
                // Screen pixel is also on - this is a collision
                if (*screenPixel == 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

// SKP Vx (Skip next instruction if key with the value Vx is pressed)
void Chip8::OP_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    // The key is pressed
    if (keypad[key]) {
        pc += 2;
    }
}

// SKNP Vx (Skip next instruction if key Vx is not pressed
void Chip8::OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    // The key is not pressed
    if (!keypad[key]) {
        pc += 2;
    }
}

// LD Vx, DT (Set Vx to be the delay timer value)
void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delayTimer;
}

// LD Vx, K (Wait for a keypress, store the value of the key in Vx)
// The easiest way to "wait" is to decrement the PC by 2 whenever a keypad
// value is not detected. This has the effect of running the same instruction repeatedly
// TODO: This is very ugly, I can fix it!
void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keypad[0]) {
        registers[Vx] = 0;
    } else if (keypad[1]) {
        registers[Vx] = 1;
    } else if (keypad[2]) {
        registers[Vx] = 2;
    } else if (keypad[3]) {
        registers[Vx] = 3;
    } else if (keypad[4]) {
        registers[Vx] = 4;
    } else if (keypad[5]) {
        registers[Vx] = 5;
    } else if (keypad[6]) {
        registers[Vx] = 6;
    } else if (keypad[7]) {
        registers[Vx] = 7;
    } else if (keypad[8]) {
        registers[Vx] = 8;
    } else if (keypad[9]) {
        registers[Vx] = 9;
    } else if (keypad[10]) {
        registers[Vx] = 10;
    } else if (keypad[11]) {
        registers[Vx] = 11;
    } else if (keypad[12]) {
        registers[Vx] = 12;
    } else if (keypad[13]) {
        registers[Vx] = 13;
    } else if (keypad[14]) {
        registers[Vx] = 14;
    } else if (keypad[15]) {
        registers[Vx] = 15;
    } else {
        pc -= 2;
    }
}

// LD DT, Vx (Set delay timer to Vx)
void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delayTimer = registers[Vx];
}

// LD ST, Vx (Set sound timer to Vx)
void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    soundTimer = registers[Vx];
}

// ADD I, Vx (Set I to I + Vx)
void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    index += registers[Vx];
}

// LD F, Vx (Set I to location of spite for digit Vx)

// We know the font characters are located at 0x50,
// and we know they're five bytes each,
// so we can get the address of the first byte of any character
// by taking an offset from the start address
void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    index = FONTSET_START_ADDRESS + (5 * digit);
}

// LD B, Vx (Store BCD representation of Vx in memory locations
// I, I+1 and I+2
// The interpreter takes the decimal value of Vx and places
// the hundreds digit in memory at location I,
// the tens digit at location I+1,
// and the ones digit at location I+2

// We can use the modulus operator to get the right-most digit of a number,
// and then do a division to remove that digit. A division by ten will either
// completely remove the digit (340 / 10 = 34), or result in a float which will be
// truncated (345 / 10 = 34.5 = 34)
void Chip8::OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    // Ones-place
    memory[index + 2] = value % 10;
    value /= 10;

    // Tens-place
    memory[index + 1] = value % 10;
    value /= 10;

    // Hundreds-place
    memory[index] = value % 10;
}

// LD [I], Vx (Store registers v0 through Vx in memory starting at location I
void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++) {
        memory[index + i] = registers[i];
    }
}

// LD Vx, [I] (Read registers v0 through Vx from memory starting at location I)
void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i) {
        registers[i] = memory[index + i];
    }
}
