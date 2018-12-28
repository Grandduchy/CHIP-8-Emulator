#ifndef CHIP8
#define CHIP8

#define HEIGHT 32
#define WIDTH 64

#include <array>
#include <string>
#include <random>
#include <fstream>
#include <stdint.h>

// Memory Map : 0x000 (0) - Start of Chip-8 ram
//              Reserved to the interpreter
//            : 0x200 (512) - Start of most Chip-8 Programs
//            : Ox600 (1536) - Start of ETI 660 Programs
//              0x200 - 0xFFF is the CHIP-8 Program / Data Space
//            : 0xFFF - End of CHIP-8 Ram



class Chip8 {
    friend class Game;
public:

    Chip8();

    void initalize();

    void loadGame(const std::string&);

    void emulateCycle();

    bool isDrawFlag() const noexcept;

    void setKeys();

    void removeDrawFlag() noexcept;

    bool isSoundFlag() const noexcept;

    void removeSoundFlag() noexcept;

private:
    uint16_t currentOpcode;
    std::array<uint8_t, 4096> memory{};
    // 16 general purpose registers (called V), 16th is a carry flag
    std::array<uint8_t, 16> registers{};
    // Program Counter holds the address to the next instruction
    // When the counter is incremented, it points to the next instruction.
    uint16_t programCounter;
    // Register to store memory addresses (called I) for later use
    // A special kind of register for memory addresses
    uint16_t indexRegister;
    // Screen is 64 by 32, where each are a pixel.
    // Graphics are black and white, 0 -> white 1 -> black
    std::array<std::array<uint8_t, WIDTH>, HEIGHT> pixels{};
    // Timers count at 60 Hz until they reach 0

    // used for timing events
    uint8_t delayTimer;
    // used for sound effects, when it reaches 0 it creates a beep
    uint8_t soundTimer;
    // Stack contains the address of the previous location after a function/subroutine call.
    // CHIP-8 allows for 16 levels of subroutines.
    std::array<uint16_t, 16> stack{};
    // pointer is set to the topmost level of the stack
    uint8_t stackPointer;

    // Used to store the state of keys
    std::array<uint8_t, 16> keys;

    // determines if the program requests to draw
    bool drawFlag = false;

    // determines if the program need to make a sound.S
    bool soundFlag = false;

    // CHIP-8 Provides a fontset for programs to display 0-F characters called sprites
    // The program needs to know where the binary representation of the sprite is located
    // where the fontset will be loaded into memory( from 0x0000).
    uint8_t fontset[80] = {
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

    void updateTimers() noexcept;

    uint8_t getRand8Bit();
};



#endif // CHIP8
