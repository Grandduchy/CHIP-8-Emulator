#include <algorithm>
#include <iostream>
#include <limits>
#include <fstream>

#include "Chip8.h"



Chip8::Chip8() {
    initalize();
}

// Initalize registers and memory once
void Chip8::initalize() {
    // reset all
    currentOpcode = 0;
    programCounter = 0x200; // program starts at 0x200
    indexRegister = 0;
    delayTimer = 0;
    soundTimer = 0;
    stackPointer = 0;

    std::fill(memory.begin(), memory.end(), 0);
    std::fill(registers.begin(), registers.end(), 0);
    std::fill(pixels.begin(), pixels.end(), 0);
    std::fill(stack.begin(), stack.end(), 0);
    std::fill(keys.begin(), keys.end(), 0);

    // load the fontset
    for (int i = 0; i < 80; i++)
        memory[0x50 + i] = fontset[i];
}

// load the game into memory
void Chip8::loadGame(const std::string& filePath) {

    std::ifstream ifs(filePath, std::ios_base::in | std::ios_base::binary);
    if (!ifs.good()) throw std::runtime_error("File does not exist");
    ifs.seekg(std::ios_base::end);
    auto size = ifs.tellg();

    if (size >= 4096 * 8) throw std::runtime_error("File is bigger than accpetable CHIP-8 memory range");
    else ifs.seekg(std::ios_base::beg);

    for(size_t i = 0; !ifs.eof(); i++) {
        uint8_t opcode;
        ifs.read(reinterpret_cast<char*>(&opcode), sizeof(uint8_t));
        memory[i + 0x200] = opcode;
    }
}

// 1. Fetch opcode
// 2. Decode opcode
// 3. Executve opcode
// 4. Update timers
void Chip8::emulateCycle() {

    uint16_t opcode = memory[programCounter] << 8 | memory[programCounter + 1];

    switch(opcode & 0xF000) { // get the leftmost bit
    case 0x0000:
        if ((opcode & 0xF000) == 0x00E0) { // 00E0 : clear screen
            std::cout << "Clear screen\n";
            std::fill(pixels.begin(), pixels.end(), 0);
            programCounter += 2;
        }
        else if ((opcode & 0xF000) == 0x00EE) {
            std::cout << "Return from function\n";
            // TODO
        }
        else {
            std::cout << "Execute subroutine\n";
            // TODO
        }
        break;
    case 0x1000:

        break;
    case 0x2000: // 0x2NNN : call subroutine at NNN
            stack[stackPointer] = programCounter;
            stackPointer++;
            programCounter = opcode & 0x0FFF;
        break;
    case 0x3000:

        break;
    case 0x4000:

        break;
    case 0x5000:

        break;
    case 0x6000:

        break;
    case 0x7000:

        break;
    case 0x8000:
        switch(opcode & 0x000F) {
            case 0x0001 :
                break;
            case 0x0002 :
                break;
            case 0x0003 :
                break;
            case 0x0004 : {// 8XY4 : add VY to VX
                uint16_t sum = registers[(opcode & 0x0F00) >> 8] + registers[(opcode & 0x00F0) >> 4];
                registers[0xf] = sum > std::numeric_limits<uint8_t>::max() ? 1 : 0; // calculate the carry

                registers[(opcode & 0x0F00) >> 8] += registers[(opcode & 0x00F0) >> 4];
                programCounter += 2;
                break;
            }
            case 0x0005 :
                break;
            case 0x0006 :
                break;
            case 0x0007 :
                break;
            case 0x000E :
                break;
            default:
                std::cerr << "Unkown opcode, expected 0x8XY(1-7,E) recieved : " << opcode << "\n";
        }

        break;
    case 0x9000:

        break;
    case 0xA000:

        break;
    case 0xB000:

        break;
    case 0xC000:

        break;
    case 0xD000:

        break;
    case 0xE000:

        break;
    case 0xF000:
        switch(opcode & 0x00FF) {
            case 0x0007 :
                break;
            case 0x000A :
                break;
            case 0x0015 :
                break;
            case 0x0018 :
                break;
            case 0x001E :
                break;
            case 0x0029 :
                break;
            case 0x0033 :
                memory[indexRegister] = registers[(opcode & 0x0F00) >> 8] / 100;
                memory[indexRegister + 1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
                memory[indexRegister + 2] = (registers[(opcode & 0x0F00) >> 8] % 100) % 10;
                programCounter += 2;
                break;
            case 0x0055 :
                break;
            case 0x0065 :
                break;
            default :
                std::cerr << "Unkown opcode, expected 0xFN(07,0A,15,18,1E,29,33,55,65) recieved : " << opcode << "\n";
        }

        break;
    default:
        std::cerr << "Unkown opcode\n";

    }



    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0) {
        if (soundTimer == 1)
            std::cout << "BEEP!\n";

        --soundTimer;
    }

}

// Determines if it needs to draw the screen
bool Chip8::isDrawFlag() const noexcept {
    return drawFlag;
}

// Pressing or releasing keys is set to the keypad
void Chip8::setKeys() {

}
