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
Chip8::Chip8(): i(0), sp(-1), pc(0), delay_timer(0), sound_timer(0),
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
    // increment and then store data.
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
    // then increment pc once again. So we will have a full 16 bit opcode.
    uint16_t opcode = (mem[pc] << 8) | mem[pc+1];
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
            }
    }
}
