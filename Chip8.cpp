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
    for (auto obj : this->pixels) {
        std::fill(obj.begin(), obj.end(), 0);
    }
    std::fill(stack.begin(), stack.end(), 0);
    std::fill(keys.begin(), keys.end(), 0);

    // load the fontset
    for (int i = 0; i < 80; i++)
        memory[static_cast<std::size_t>(i)] = fontset[i];
}

// load the game into memory
void Chip8::loadGame(const std::string& filePath) {

    std::ifstream ifs(filePath, std::ios_base::in | std::ios_base::binary);
    if (!ifs.good())
        throw std::runtime_error("File does not exist");
    ifs.seekg(std::ios_base::end);
    auto size = ifs.tellg();

    if (size >= (0xFFF - 0x200) * std::numeric_limits<uint8_t>::max())
        throw std::runtime_error("File is bigger than accpetable CHIP-8 memory range");
    else ifs.seekg(std::ios_base::beg);
    std::cout << std::boolalpha;
    std::cout << std::hex;

    uint8_t opcode;
    for (size_t i = 0; ifs.read(reinterpret_cast<char*>(&opcode), sizeof(uint8_t)) ; i++) {
        memory[i + 0x200] = opcode;
    }
}

void Chip8::removeDrawFlag() noexcept {
    drawFlag = false;
}

// 1. Fetch opcode
// 2. Decode opcode
// 3. Executve opcode
// 4. Update timers
void Chip8::emulateCycle() {
    std::cerr << std::hex;
    uint16_t opcode = static_cast<uint16_t>(memory[programCounter] << 8 | memory[programCounter + 1]);
    switch(opcode & 0xF000) { // get the leftmost bit
    case 0x0000:
        switch(opcode & 0x00FF) {
            case 0x00E0 : // 00E0 : clear screen
                for (auto obj : pixels) {
                    std::fill(obj.begin(), obj.end(), 0);
                }
                drawFlag = true;
                programCounter += 2;
            break;
            case 0x00EE : // 00EE : return from subroutine
                programCounter = stack[--stackPointer];
                programCounter += 2;
            break;
            default :
                std::cerr << "Unsupported 0xNNN opcode recieved : " << opcode << "\n";
            break;
        }
        break;
    case 0x1000: // 1NNN : jump to address NNN
        programCounter = 0x0FFF & opcode;
        break;
    case 0x2000: // 2NNN : call subroutine at NNN
        stack.at(stackPointer) = programCounter;
        stackPointer++;
        programCounter = opcode & 0x0FFF;
        break;
    case 0x3000: { // 3XNN : skip if VX equal NN
        uint8_t reg = registers.at((0x0F00 & opcode) >> 8);
        uint8_t num = 0x00FF & opcode;
        programCounter += reg == num ? 4 : 2;
        break;
    }
    case 0x4000: {// 4XNN : skip if VX does not equal NN
        uint8_t reg = registers.at((0x0F00 & opcode) >> 8);
        uint8_t num = 0x00FF & opcode;
        programCounter += reg != num ? 4 : 2;
        break;
    }
    case 0x5000:
        if ((opcode & 0x000F) == 0) { // 5XY0 : skip if VX == VY
            uint8_t VX = registers.at((0x0F00 & opcode) >> 8);
            uint8_t VY = registers.at((0x00F0 & opcode) >> 4);
            programCounter += VX == VY ? 4 : 2;
        }
        else {
            std::cerr << "Invalid opcode expected 0x5XY0 instead recieved : " << opcode << "\n";
        }
        break;
    case 0x6000: // 6XNN : set VX to NN
        registers.at((opcode & 0x0F00) >> 8) = opcode & 0x00FF;
        programCounter += 2;
        break;
    case 0x7000: // 7XNN : adds NN to VX
        registers.at((opcode & 0x0F00) >> 8) += (opcode & 0x00FF);
        programCounter += 2;
        break;
    case 0x8000:
        switch(opcode & 0x000F) {
            case 0x0000 : // 8XY0 : set VX to VY
                registers.at((opcode & 0x0F00) >> 8) = registers.at((opcode & 0x00F0) >> 4);
                programCounter += 2;
                break;
            case 0x0001 :  // 8XY1 : set VX to binary OR with VX and VY
                registers.at((opcode & 0x0F00) >> 8) |= registers.at((opcode & 0x00F0) >> 4);
                programCounter += 2;
                break;
            case 0x0002 :  // 8XY2 : set VX to binary AND with VX and VY
                registers.at((opcode & 0x0F00) >> 8) &= registers.at((opcode & 0x00F0) >> 4);
                programCounter += 2;
                break;
            case 0x0003 :  // 8XY3 : set VX to binary XOR with VX and VY
                registers.at((opcode & 0x0F00) >> 8) |= registers.at((opcode & 0x00F0) >> 4);
                programCounter += 2;
                break;
            case 0x0004 : {// 8XY4 : add VY to VX
                uint16_t sum = registers.at((opcode & 0x0F00) >> 8) + registers.at((opcode & 0x00F0) >> 4);
                std::get<0xF>(registers) = sum > std::numeric_limits<uint8_t>::max() ? 1 : 0; // calculate the carry

                registers.at((opcode & 0x0F00) >> 8) = static_cast<uint8_t>(sum);
                programCounter += 2;
                break;
            }
            case 0x0005 :  {// 8XY5 : subtract VY from VX and set to VX
                int16_t sum = registers.at((opcode & 0x0F00) >> 8) - registers.at((opcode & 0x00F0) >> 4);
                std::get<0xF>(registers) = 1;
                if (sum <= -1) { // we need to borrow from VF
                    if (std::get<0xF>(registers) == 1) {// Can borrow
                        std::get<0xF>(registers) = 0;
                        registers.at((opcode & 0x0F00) >> 8) = static_cast<uint8_t>(sum + std::numeric_limits<uint8_t>::max());
                    }
                    else { // Can't borrow
                        registers.at((opcode & 0x0F00) >> 8) = static_cast<uint8_t>(sum);
                    }
                }
                else {

                    registers.at((opcode & 0x0F00) >> 8) = static_cast<uint8_t>(sum);
                }
                programCounter += 2;
                break;
            }
            case 0x0006 : // 8XY6 : right shift VY by one and store in VX
                std::get<0xF>(registers) = registers.at((opcode & 0x00F0) >> 4) & 1; // store the least significant bit of VY in VF
                registers.at((opcode & 0x0F00) >> 8) = registers.at((opcode & 0x00F0) >> 4 ) >> 1;
                programCounter +=2;
                break;
            case 0x0007 : {// 8XY7 : subtract VX from VY and set to VX
                int16_t sum = registers.at((opcode & 0x00F0) >> 4) - registers.at((opcode & 0x0F00) >> 8);
                std::get<0xF>(registers) = 1;
                if (sum <= -1) {
                    if (std::get<0xF>(registers) == 1) {
                        std::get<0xF>(registers) = 0;
                        registers.at((registers.at((opcode & 0x0F00) >> 8))) = static_cast<uint8_t>(sum + std::numeric_limits<uint8_t>::max());
                    }
                    else {
                        registers.at((registers.at((opcode & 0x0F00) >> 8))) = static_cast<uint8_t>(sum);
                    }
                }
                else {
                    registers.at((registers.at((opcode & 0x0F00) >> 8))) = static_cast<uint8_t>(sum);
                }

                programCounter += 2;
                break;
            }
            case 0x000E :// 8XYE : left shift VY by one and store in VX
                std::get<0xF>(registers) = (registers.at((opcode & 0x00F0) >> 4) & (1 << 8)) >> 8; // store the greatest significant bit of VY in VF
                registers.at((opcode & 0x0F00) >> 8) = static_cast<uint8_t>(1 << registers.at((opcode & 0x00F0) >> 4));
                programCounter += 2;
                break;
            default:
                std::cerr << "Unkown opcode, expected 0x8XY(1-7,E) recieved : " << opcode << "\n";
        }

        break;
    case 0x9000:
        if ((opcode & 0x000F) == 0) { // 9XY0 : skip if VX != VY
            uint8_t VX = registers.at((opcode & 0x0F00) >> 8);
            uint8_t VY = registers.at((opcode & 0x00F0) >> 4);
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
        programCounter = registers[0] + (opcode & 0x0FFF);
        break;
    case 0xC000: // CXNN : set VX to a random number Binary ANDed by NN, random number is unsigned 8 bits.
        registers.at((opcode & 0x0F00) >> 8) = getRand8Bit() & (opcode & 0x00FF);
        programCounter += 2;
        break;
    case 0xD000: {// DXYN : Draw sprite at VX, VY of N height starting at I.
        uint8_t xPos = registers.at((opcode & 0x0F00) >> 8);
        uint8_t yPos = registers.at((opcode & 0x00F0) >> 4);
        uint8_t height = opcode & 0x000F;
        uint8_t width = 8;

        std::get<0xF>(registers) = 0;

        for (uint8_t y = 0; y != height; y++) {
            // The I register points to the binary representation of the sprite
            uint8_t binaryImage = memory.at(indexRegister + y);
            // go through each bit
            for (uint8_t x = 0; x != width; x++) {
                uint8_t mask = 0x80 >> x; // start from greatest bit to least, 0x80 is the greatest bit in uint_8
                bool isOn = binaryImage & mask;

                // The pixel must wrap around if it goes out of scope
                uint8_t pixelY = (yPos + y) % HEIGHT;
                uint8_t pixelX = (xPos + x) % WIDTH;

                uint8_t& pixel = pixels[pixelY][pixelX];
                if (isOn) {
                    if (pixel == 1)
                        std::get<0xF>(registers) = 1;
                    pixel ^= 1;
                }

            }

        }
        drawFlag = true;
        programCounter += 2;
        break;
    }
    case 0xE000:
        switch(opcode & 0x00FF) {
            case 0x009E : {// EX9E : skip if key stored in VX is turned on
                // this should be changed in the future if its really a boolean condition for keys
                // since currently its an integer.
                bool isOn = registers.at((opcode & 0x0F00) >> 8) == 1;
                programCounter += isOn ? 4 : 2;
                break;
            }
            case 0x00A1 : {// EXA1 : skip if key stored in VX isn't pressed
                 bool isOn = registers.at((opcode & 0x0F00) >> 8) == 1;
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
                registers.at((opcode & 0x0F00) >> 8) = delayTimer;
                programCounter += 2;
                break;
            case 0x000A : {// FX0A : wait until a key is pressed and store to VX
                auto it = std::find_if_not(keys.cbegin(), keys.cend(), [](const uint8_t& i) {
                    return i == 0;
                });
                if (it != keys.end()) {
                    registers.at( (opcode & 0x0F00) >> 8) = static_cast<uint8_t>(std::distance(keys.cbegin(), it));
                    programCounter += 2;
                }
                break;
            }
            case 0x0015 : // FX15 : Set the delay timer to VX
                delayTimer = registers.at((opcode & 0x0F00) >> 8);
                programCounter += 2;
                break;
            case 0x0018 : // FX18 : Set the sound timer to VX
                soundTimer = registers.at((opcode & 0x0F00) >> 8);
                programCounter += 2;
                break;
            case 0x001E : {// FX1E : adds VX to I
                if (indexRegister + registers.at((opcode & 0x0F00) >> 8) > 0xFFF)
                    std::get<0xF>(registers) = 1;
                else
                    std::get<0xF>(registers) = 0;
                indexRegister += registers.at((opcode & 0x0F00) >> 8);
                programCounter += 2;
            }
                break;
            case 0x0029 : {// FX29 : Set I to the sprite location for a character in VX
                uint8_t character = registers.at((0x0F00 & opcode) >> 8);
                indexRegister = character * 5;
                programCounter += 2;
                break;
            }
            case 0x0033 : // FX33 : Store the BCD of VX to I
                memory.at(indexRegister) = registers.at((opcode & 0x0F00) >> 8) / 100;
                memory.at(indexRegister + 1) = (registers.at((opcode & 0x0F00) >> 8) / 10) % 10;
                memory.at(indexRegister + 2) = (registers.at((opcode & 0x0F00) >> 8) % 100) % 10;
                programCounter += 2;
                break;
            case 0x0055 : { // FX55 : Write registers into memory starting from V0 to (including) VX starting at I, I is incremented by 1.
                auto endRegister = registers.cbegin() + ((0x0F00 & opcode) >> 8) + 1;
                for (auto it = registers.cbegin(); it != endRegister; it++) {
                    auto pos = std::distance(registers.cbegin(), it);
                    memory.at(indexRegister++) = static_cast<uint8_t>(registers.at(static_cast<std::size_t>(pos)));
                }
                programCounter += 2;
                break;
            }
            case 0x0065 : {// FX65 Read registers
                auto endRegister = registers.cbegin() + ((0x0F00 & opcode) >> 8) + 1;
                for (auto it = registers.cbegin(); it != endRegister; it++) {
                       auto pos = std::distance(registers.cbegin(), it);
                        registers.at(static_cast<std::size_t>(pos)) = memory.at(indexRegister++);
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
    static std::default_random_engine engine(device());
    static std::uniform_int_distribution<short> distance(0,255); // <-- undefined behavior if short is uint8_t
    return static_cast<uint8_t>(distance(engine));
}

// Determines if it needs to draw the screen
bool Chip8::isDrawFlag() const noexcept {
    return drawFlag;
}

// Pressing or releasing keys is set to the keypad
void Chip8::setKeys() {

}
