#include <algorithm>
#include <iostream>
#include <limits>
#include <fstream>

#include "Chip8.hpp"

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
    std::cerr << std::hex;
    uint16_t opcode = memory[programCounter] << 8 | memory[programCounter + 1];

    switch(opcode & 0xF000) { // get the leftmost bit
    case 0x0000:
        if ((opcode & 0xF000) == 0x00E0) { // 00E0 : clear screen
            std::cout << "Clear screen\n";
            std::fill(pixels.begin(), pixels.end(), 0);
            programCounter += 2;
        }
        else if ((opcode & 0xF000) == 0x00EE) { // 00EE : return from subroutine
            programCounter = stack[--stackPointer];
            programCounter += 2;
        }
        else {
            std::cerr << "Unsupported 0xNNN opcode\n";
        }
        break;
    case 0x1000: // 1NNN : jump to address NNN
        programCounter = 0x0FFF & opcode;

        break;
    case 0x2000: // 2NNN : call subroutine at NNN
            stack[stackPointer] = programCounter;
            stackPointer++;
            programCounter = opcode & 0x0FFF;
        break;
    case 0x3000: { // 3XNN : skip if equal
            uint8_t reg = 0x0F00 & opcode;
            uint8_t num = 0x00FF & opcode;
            programCounter += reg == num ? 4 : 2;
    }
        break;
    case 0x4000: {// 4XNN : skip if not equal
            uint8_t reg = 0x0F00 & opcode;
            uint8_t num = 0x00FF & opcode;
            programCounter += reg != num ? 4 : 2;
    }
        break;
    case 0x5000:
        if ((opcode & 0xFFF) != 0) { // 5XY0 : skip if VX == VY
            uint8_t reg1 = 0x0F00 & opcode, reg2 = 0x00F0 & opcode;
            programCounter += reg1 == reg2 ? 4 : 2;
        }
        else {
            std::cerr << "Invalid Opcode expected 0x5XY0 instead recieved : " << opcode << "\n";
        }
        break;
    case 0x6000: // 6XNN : set VX to NN
        registers[(opcode & 0x0F00) >> 2] = opcode & 0x00FF;
        programCounter += 2;
        break;
    case 0x7000: // 7XNN : adds NN to VX
        registers[(opcode & 0x0F00) >> 2] += (opcode & 0x00FF);
        programCounter += 2;
        break;
    case 0x8000:
        switch(opcode & 0x000F) {
            case 0x0000 : // 8XY0 : set VX to VY
                registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
                programCounter += 2;
                break;
            case 0x0001 :  // 8XY1 : set VX to binary OR with VX and VY
                registers[(opcode & 0x0F00) >> 8] |= registers[(opcode & 0x00F0) >> 4];
                programCounter += 2;
                break;
            case 0x0002 :  // 8XY2 : set VX to binary AND with VX and VY
                registers[(opcode & 0x0F00) >> 8] &= registers[(opcode & 0x00F0) >> 4];
                programCounter += 2;
                break;
            case 0x0003 :  // 8XY3 : set VX to binary XOR with VX and VY
                registers[(opcode & 0x0F00) >> 8] |= registers[(opcode & 0x00F0) >> 4];
                programCounter += 2;
                break;
            case 0x0004 : {// 8XY4 : add VY to VX
                uint16_t sum = registers[(opcode & 0x0F00) >> 8] + registers[(opcode & 0x00F0) >> 4];
                registers[0xf] = sum > std::numeric_limits<uint8_t>::max() ? 1 : 0; // calculate the carry

                registers[(opcode & 0x0F00) >> 8] = sum;
                programCounter += 2;
                break;
            }
            case 0x0005 :  {// 8XY5 : subtract VY from VX and set to VX
                int16_t sum = registers[(opcode & 0x0F00) >> 8] - registers[(opcode & 0x00F0) >> 4];
                if (sum <= -1) { // we need to borrow from VF
                    if (registers[0xf] == 1) {// Can borrow
                        registers[0xf] = 0;
                        registers[(opcode & 0x0F00) >> 8] = sum + std::numeric_limits<uint8_t>::max();
                    }
                    else { // Can't borrow
                        registers[0xf] = 1;
                        registers[(opcode & 0x0F00) >> 8] = sum;
                    }
                }
                else {
                    registers[(opcode & 0x0F00) >> 8] = sum;
                }
                programCounter += 2;
                break;
            }
            case 0x0006 : // 8XY6 : right shift VY by one and store in VX
                registers[0xF] = registers[(opcode & 0x00F0) >> 4] & 1; // store the least significant bit of VY in VF
                registers[(opcode& 0x0F00) >> 8] = (registers[(opcode & 0x00F0) >> 4 ] >> 1);
                programCounter +=2;
                break;
            case 0x0007 : {// 8XY7 : subtract VX from VY and set to VX
                int16_t sum = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];
                if (sum <= -1) {
                    if (registers[0xF] == 1) {
                        registers[0xF] = 0;
                        registers[(registers[(opcode & 0x0F00) >> 8])] = sum + std::numeric_limits<uint8_t>::max();
                    }
                    else {
                        registers[0xF] = 1;
                        registers[(registers[(opcode & 0x0F00) >> 8])] = sum;
                    }
                }
                else {
                    registers[(registers[(opcode & 0x0F00) >> 8])] = sum;
                }

                programCounter += 2;
                break;
            }
            case 0x000E :// 8XYE : left shift VY by one and store in VX
                registers[0xF] = (registers[(opcode & 0x00F0) >> 4] & (1 << 8)) >> 8; // store the greatest significant bit of VY in VF
                registers[(opcode & 0x0F00) >> 8] = (1 << registers[(opcode & 0x00F0) >> 4]);
                programCounter += 2;
                break;
            default:
                std::cerr << "Unkown opcode, expected 0x8XY(1-7,E) recieved : " << opcode << "\n";
        }

        break;
    case 0x9000:
        if (opcode & 0x000F != 0) { // 9XY0 : skip if VX != VY
            uint8_t VX = registers[(opcode & 0x0F00) >> 8];
            uint8_t VY = registers[(opcode & 0x00F0) >> 4];
            programCounter += VX != VY ? 4 : 2;
        }
        else {
            std::cerr << "Unkown opcode, expected 0x9XY0 recieved : " << opcode << "\n";
        }
        break;
    case 0xA000: // ANN : set I to NNN
        indexRegister = opcode & 0x0FFF;
        programCounter += 2;
        break;
    case 0xB000: // BNNN : set/jump to V0 and NNN
        programCounter = registers[0] + opcode& 0x0FFF;
        break;
    case 0xC000: // CXNN : set VX to a random number Binary ANDed by NN, random number is unsigned 8 bits.
        registers[(opcode & 0x0F00) >> 8] = getRand8Bit() & (opcode & 0x00FF);
        programCounter += 2;
        break;
    case 0xD000: {// DXYN : Draw sprite at VX, Vy
        // TODO
        break;
    }
    case 0xE000:
        switch(opcode & 0x00FF) {
            case 0x009E : {// EX9E : skip if key stored in VX is turned on
                // this should be changed in the future if its really a boolean condition for keys
                // since currently its an integer.
                bool isOn = registers[(opcode & 0x0F00) >> 8] == 1;
                programCounter += isOn ? 4 : 2;
                break;
            }
            case 0x00A1 : {// EXA1 : skip if key stored in VX isn't pressed
                 bool isOn = registers[(opcode & 0x0F00) >> 8] == 1;
                 programCounter += isOn ? 2 : 4;
                 break;
            }
            default :
            std::cerr << "Unkown opcode, expected 0xEX(9E, A1) recieved : " << opcode << "\n";
        }

        break;
    case 0xF000:
        switch(opcode & 0x00FF) {
            case 0x0007 : // FX07 : Set VX to the delay timer
                registers[(opcode & 0x0F00) >> 8] = delayTimer;
                programCounter += 2;
                break;
            case 0x000A : {// FX0A : wait until a key is pressed and store to VX
                auto it = std::find_if_not(keys.cbegin(), keys.cend(), [](const uint8_t& i) {
                    return i == 0;
                });
                if (it != keys.end()) {
                    registers[ (opcode & 0x0F00) >> 8] = std::distance(keys.cbegin(), it);
                    programCounter += 2;
                }
                break;
            }
            case 0x0015 : // FX15 : Set the delay timer to VX
                delayTimer = registers[(opcode & 0x0F00) >> 8];
                programCounter += 2;
                break;
            case 0x0018 : // FX18 : Set the sound timer to VX
                soundTimer = registers[(opcode & 0x0F00) >> 8];
                programCounter += 2;
                break;
            case 0x001E : // FX1E : adds VX to I
                indexRegister += registers[(opcode & 0x0F00) >> 8];
                programCounter += 2;
                break;
            case 0x0029 : // FX29 : Set I to the sprite location for a character to VX
                // TODO
                break;
            case 0x0033 :
                memory[indexRegister] = registers[(opcode & 0x0F00) >> 8] / 100;
                memory[indexRegister + 1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
                memory[indexRegister + 2] = (registers[(opcode & 0x0F00) >> 8] % 100) % 10;
                programCounter += 2;
                break;
            case 0x0055 : { // FX55 : Write registers into memory starting from V0 to (including) VX starting at I, I is incremented by 1.
                auto endRegister = registers.cbegin() + ((0x0F00 & opcode) >> 8) + 1; // <- Note to check this, +1 may not be needed.
                for (auto it = registers.cbegin(); it != endRegister; it++) {
                    auto pos = std::distance(registers.cbegin(), it);
                    memory[indexRegister++] = registers[pos];
                }
                programCounter += 2;
                break;
            }
            case 0x0065 : {// FX65 Read registers
                auto endRegister = registers.cbegin() + ((0x0F00 & opcode) >> 8) + 1;
                for (auto it = registers.cbegin(); it != endRegister; it++) {
                       auto pos = std::distance(registers.cbegin(), it);
                        registers[pos] = memory[indexRegister++];
                }
                programCounter += 2;
                break;
            }
            default :
                std::cerr << "Unkown opcode, expected 0xFN(07,0A,15,18,1E,29,33,55,65) recieved : " << opcode << "\n";
        }

        break;
    default:
        std::cerr << "Unkown opcode\n";

    }
    updateTimers();

    std::cerr << std::dec << std::flush;
}


bool isHalted() const noexcept;

void Chip8::updateTimers() noexcept {
    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0) {
        if (soundTimer == 1)
            std::cout << "BEEP!\n";

        --soundTimer;
    }
}

uint8_t Chip8::getRand8Bit() {
    static std::random_device device;
    static std::default_random_engine engine;
    static std::uniform_int_distribution<short> distance(0,255); // <-- undefined behavior if short is uint8_t
    return distance(engine);
}

// Determines if it needs to draw the screen
bool Chip8::isDrawFlag() const noexcept {
    return drawFlag;
}

// Pressing or releasing keys is set to the keypad
void Chip8::setKeys() {

}
