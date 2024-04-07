#include <string>
#include <fstream>
#include <iostream>

#include "cpu.hpp"

void Chip8::loadRom(std::string rom_file) {
    std::fstream rom(rom_file, std::ios::in | std::ios::binary | std::ios::ate);

    if (!rom) {
        std::cerr << "Failed to read rom file." << std::endl;
        return;
    }

    int file_size = rom.tellg();
    char *data_buf = new char[file_size];

    rom.seekg(0, std::ios::beg);
    rom.read(data_buf, file_size); // read the contents of the file into the data_buf
                                   // first arg - buffer, second arg - no of bytes to read in.
    rom.close();

    for (int i = 0; i < file_size; i++) {
        // the starting for the programs is 512(0x200)
        // with that offset store the data.
        mem[0x200 + i] = data_buf[i];
    }

    delete[] data_buf; 
}

// stack pointer(sp) should be -1 and others to default (0) or empty values.
Chip8::Chip8(): i(0), sp(-1), pc(0x200), delay_timer(0), sound_timer(0),
                v{}, mem{}, stack{}, keymap{}, video{} {
    const uint8_t FONT_SIZE = 80;

    uint8_t fonts[FONT_SIZE] = {
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

    // load the above fonts from the location 050 to 09F
    for (int i=0; i < 80; i++) {
        mem[0x050 + i] = fonts[i];
    }
}

uint16_t Chip8::stackPop() {
    // store the top of the stack in data and return the data. 
    uint16_t data = stack[sp--];
    return data;
}

void Chip8::stackPush(uint16_t &data) {
    // increment sp and then store data.
    stack[++sp] = data;
}

void Chip8::timers() {
    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        --sound_timer;
    }
}

void Chip8::machine() {
    // read the first 8 bits and then increment pc
    // then again read another 8 bits from the memory
    // then increment pc once again. So we will have a full 16-bit opcode.
    uint16_t opcode = (mem[pc] << 8) | mem[pc+1];
    std::cout << std::hex << opcode << std::endl;
    pc += 2;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x0FFF) {
                case 0x0E0: //clear the display(video output)
                    for (int i=0; i < 2048; i++) {
                        video[i] = 0;
                    }
                    break;
                case 0x00EE: // return from a subroutine
                    pc = stackPop();
                    break;
                default:
                    std::cerr << "unknown instruction." << std::endl;
            }
            break;
        case 0x1000: // goto NNN (like jump unconditionally.)
            // take the lowest three nibbles and set it to current pc
            pc = opcode & 0xFFF;
            break;
        case 0x2000: // function call 
            stackPush(pc);
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // if Vx == kk then skip the next instruction
            // for skiping the next instruction increment the pc by 2.
            if (v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 2;
            }
            break;
        case 0x4000: // if Vx != kk then skip the next instruction
            if (v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc +=2;
            }
            break;
        case 0x5000: // skip next ins if Vx = Vy
            if (v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 8]) {
                pc += 2;
            }
            break;
        case 0x6000: // load Vx = kk
            v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            break;
        case 0x7000: // add the value the kk from the ins to the Vx register
            v[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: // store the value of Vy to Vx register
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0001: // perform OR operation on Vx, Vy then store the result in Vx 
                    v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0002: // perform AND operaton on Vx, Vy then store the result in Vx 
                    v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0003: // XOR: Vx = Vx ^ Vy (V? means register)
                    v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0004: {// ADD: Vx = Vx + Vy, VF = 1 if overflow, otherwise 0
                    uint16_t val = v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4];
                    if (val > 255) { // just an 8-bit register. thus, the range is 0-255
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    } 
                    v[(opcode & 0x0F00) >> 8] = val & 0x00FF; // mask lower byte (could even just use 0xFF)
                    break;
                }
                case 0x0005: // subtract Vy from Vx and store the result in Vx, and VF set to 0 when there is a borrow, if not set it to 1.
                    if (v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4]) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0006: // SHIFT RIGHT: if the lsb of value in the register(Vx) is 1 then set VF = 1, else don't. then shift Vx right by 1-bit
                    // TODO: replace the below 'if' with single line.
                    if ((v[(opcode & 0x0F00) >> 8] & 0x1) == 0x1) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[(opcode & 0x0F00) >> 8] >>= 1;
                    break;
                case 0x0007: // subtract Vx from Vy and store the result in Vx. if Vy > Vx set VF = 1 otherwise VF = 0
                    if (v[(opcode & 0x0F00) >> 8] < v[(opcode & 0x00F0) >> 4]) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x000E: // SHIFT LEFT: if msb of value in the register is 1 then VF = 1 otherwise don't. 
                    // TODO: replace the below 'if' with single line.
                    if ((v[(opcode & 0x0F00) >> 8] & 0x10) == 0x10) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[(opcode & 0x0F00) >> 8] <<= 1;
                    break;
                default:
                    std::cerr << "unknown instruction" << std::endl;
                    break;
            }
            break;
        case 0x9000:
            if (v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4]) {
                pc += 2;
            }
            break;
        default:
            std::cerr << "Invalid opcode" << std::endl;
    }

    timers();
}
