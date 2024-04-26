#include <string>
#include <fstream>
#include <iostream>
#include <cstring>

#include "chip8.hpp"

Chip8::Chip8() {
    // set the default values for the interpreter components.
    std::memset(v, 0, NUM_REGS);
    std::memset(stack, 0, STACK_SIZE);
    std::copy(fonts, fonts + FONT_SIZE, mem + 0x50);
    index = 0;
    sp = -1; // set the stack pointer to -1 means empty.
    pc = START_ADDR;
    delay_timer = 0;
    sound_timer = 0;
    std::memset(graphics, false, SCREEN_WIDTH * SCREEN_HEIGHT);
    std::memset(keys, false, NUM_KEYS);
}

bool Chip8::loadRom(const std::string& rom_file) {
    std::fstream rom(rom_file, std::ios::in | std::ios::binary | std::ios::ate);

    if (!rom) {
        std::cerr << "failed to read rom file." << std::endl;
        return false;
    }

    const int FILE_SIZE = rom.tellg();

    // check if the rom file is either too big or empty.
    if (FILE_SIZE > sizeof(mem)/sizeof(mem[0])) {
        std::cerr << "rom file is too big to load." << std::endl;
        return false;
    } else if (FILE_SIZE <= 0) {
        std::cerr << "empty rom file detected..." << std::endl;
        return false;
    }

    char *data_buf = new char[FILE_SIZE];
    rom.seekg(0);
    rom.read(data_buf, FILE_SIZE); // read the contents of the file into the data_buf
                                   // first arg - buffer, second arg - no of bytes to read in.
    rom.close();

    // starting address for the program is 512kb(0x200)
    // with that offset, load the rom data.
    std::copy(data_buf, data_buf + FILE_SIZE, mem+0x200);

    delete[] data_buf; 
    return true;
}

uint16_t Chip8::pop() {
    return stack[sp--]; // takes the top value from stack, decrement the sp then return. 
}

void Chip8::push(const uint16_t data) {
    stack[++sp] = data;
}

void Chip8::timers() {
    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        if (sound_timer == 1) {
            std::cout << "BEEP!" << std::endl;
            //TODO: beep
        }
        --sound_timer;
    }
}

bool* Chip8::keyPad() {
    return keys;
}

bool* Chip8::getGraphics() {
    return graphics;
}

void Chip8::cycle() {
    // (fetch phase)
    // for 16 bit opcode. read 8 bits from the current pc
    // and another 8 bits from the pc+1
    uint16_t opcode = (mem[pc] << 8) | mem[pc+1];
    pc += 2;

    bool invalid = false;

    // (decode and execute phase)
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0: //clear the screen 
                    std::memset(graphics, false, SCREEN_WIDTH * SCREEN_HEIGHT);
                    break;
                case 0x00EE: // return from a subroutine
                    pc = pop();
                    break;
                default:
                    invalid = true;
            }
            break;
        case 0x1000: // goto NNN (like jump unconditionally.)
            // take the lowest three nibbles and set it to current pc
            pc = opcode & 0x0FFF;
            break;
        case 0x2000: // function call 
            push(pc);
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // if Vx == kk then skip the next instruction
            // to skip the next instruction increment the pc by 2.
            if (v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 2;
            }
            break;
        case 0x4000: // if Vx != kk then skip the next instruction
            if (v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc +=2;
            }
            break;
        case 0x5000: // skip next instruction if Vx = Vy
            if (v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4]) {
                pc += 2;
            }
            break;
        case 0x6000: // load Vx = kk
            v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            break;
        case 0x7000: // add the value kk from the ins to the Vx register
            v[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: // Vx = Vy
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0001: // Vx = Vx | Vy
                    v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0002: // Vx = Vx & Vy
                    v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0003: // XOR: Vx = Vx ^ Vy
                    v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0004: {// ADD: Vx = Vx + Vy, VF = 1 if overflow, otherwise 0
                    uint16_t val = v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4];
                    if (val > UINT8_MAX) { // just an 8-bit register. thus, the range is 0-255
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    } 
                    v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
                    break;
                }
                case 0x0005: // subtract Vy from Vx and store the result in Vx, and VF set to 0 when there is a borrow, if not set it to 1.
                    if (v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4]) {
                        // flag work opposite to add
                        v[0xF] = 1; // no borrow
                    } else {
                        v[0xF] = 0; // borrow
                    }
                    v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0006: // SHIFT RIGHT: if the lsb of value in the register(Vx) is 1 then set VF = 1 else 0 
                    v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
                    v[(opcode & 0x0F00) >> 8] >>= 1;
                    break;
                case 0x0007: // subtract Vx from Vy and store the result in Vx. if Vy > Vx set VF = 1 otherwise VF = 0
                    if (v[(opcode & 0x0F00) >> 8] < v[(opcode & 0x00F0) >> 4]) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
                    break;
                case 0x000E: // SHIFT LEFT: if msb of value in the register is 1 then VF = 1 otherwise 0.
                    v[0xF] = (v[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
                    v[(opcode & 0x0F00) >> 8] <<= 1;
                    break;
                default:
                    invalid = true;
            }
            break;
        case 0x9000: // skips the next instruction if Vx doesn't equal to Vy
            if (v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4]) {
                pc += 2;
            }
            break;
        case 0xA000: // set index to address nnn
            index = (opcode & 0x0FFF);
            break;
        case 0xB000: // jump to address nnn + V0
            pc = (opcode & 0x0FFF) + v[0];
            break;
        case 0xC000: // perform AND with random byte and nn and store result in Vx
            srand(time(nullptr));
            v[(opcode & 0x0F00) >> 8] = ((rand() % UINT8_MAX) & (opcode & 0x00FF));
            break;
        case 0xD000: { // display the sprite at the location Vx, Vy
            // if the (x, y) gets outside the 64x32 it will get wrapped around.
            uint8_t x_pos = v[(opcode & 0x0F00) >> 8];
            uint8_t y_pos = v[(opcode & 0x00F0) >> 4];
            uint8_t height = (opcode & 0x000F); // this tells how tall or the number of rows.

            // dir means direction not directory atleast here.
            //     col(x_dir)
            // ------------------
            // |                |
            // |                | row(y_dir)
            // |                |
            // ------------------

            bool is_flip = false;
            for (int y_dir = 0; y_dir < height; y_dir++) {
                uint8_t pixels = mem[index + y_dir];
                for (int x_dir = 0; x_dir < 8; x_dir++) {
                    if (pixels & (0x80 >> x_dir)) {
                        auto x = (x_pos + x_dir) % SCREEN_WIDTH;
                        auto y = (y_pos + y_dir) % SCREEN_HEIGHT;

                        auto idx = x + SCREEN_WIDTH * y;
                        is_flip |= graphics[idx];
                        graphics[idx] ^= true;
                    }
                }
            }
            if (is_flip)
                v[0xF] = 1;
            else
                v[0xF] = 0;
            break;
        }
        case 0xE000:
            switch (opcode & 0x00FF) {
                // TODO: check for correctness
                case 0x009E: // skip next instruction if key stored in Vx is pressed. 
                    if (keys[v[(opcode & 0x0F00) >> 8]]) 
                        pc += 2;
                    break;
                case 0x00A1: // skip an instruction if the key stored in Vx is not pressed
                    if (!keys[v[(opcode & 0x0F00) >> 8]])
                        pc += 2; 
                    break;
                default:
                    invalid = true;
            }            
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // set Vx = delay_timer
                    v[(opcode & 0x0F00) >> 8] = delay_timer;
                    break;
                case 0x000A: { // wait for the keypress and store the value of key in Vx
                    // TODO: check for correctness.
                    bool pressed = false;
                    
                    // iterate through the elements in the 'keys' 
                    // if the key is pressed set the flag(pressed) to true
                    // and set the value of current index to the Vx; 
                    for (int i = 0; i < UINT8_MAX; i++) {
                        if (keys[i]) {
                            pressed = true;
                            v[(opcode & 0x0F00) >> 8] = i;
                            break;
                        }
                    }

                    // if the key is not pressed, repeat the same instruction
                    if (!pressed) 
                        pc -= 2;
                    break;
                }
                case 0x0015: // set DT to Vx
                    delay_timer = v[(opcode & 0x0F00) >> 8];
                    break;
                case 0x0018: // set ST to Vx
                    sound_timer = v[(opcode & 0x0F00) >> 8];
                    break;
                case 0x001E: // I = I + Vx
                    // TODO: check if need to add overflow check constraint index += v[(opcode & 0x0F00 >> 8)];
                    index += v[(opcode & 0x0F00) >> 8];
                    break;
                case 0x0029: // set index register to location of sprite
                             // for digit in Vx
                    // font sprites starts from 0x50 in the memory
                    // each sprite is 5 bytes long.
                    index = 0x050 + (v[(opcode & 0x0F00) >> 8] * 0x5) ;
                    break;
                case 0x0033: // store BCD representation of Vx in the memory  
                    mem[index] = v[(opcode & 0x0F00) >> 8] / 100;
                    mem[index + 1] = (v[(opcode & 0x0F00) >> 8] % 100) / 10;
                    mem[index + 2] = (v[(opcode & 0x0F00) >> 8]) % 10;
                    break;
                case 0x0055: { // store registers V0 - Vx in memory at index
                    uint8_t x = (opcode & 0xF00) >> 8;
                    for (int idx = 0; idx <= x; idx++) {
                        mem[index + idx] = v[idx];
                    }
                    break;
                }
                case 0x0065: { // load V0 - Vx with values from the memory at index
                    uint8_t x = (opcode & 0xF00) >> 8;
                    for (int idx = 0; idx <= x; idx++) {
                        v[idx] = mem[index + idx];
                    }
                    break;
                }
                default:
                    invalid = true;
            }
            break;
        default:
            invalid = true;
    }

    if (invalid) {
        std::cerr << "Invalid Opcode: " << std::hex << opcode << std::endl;
    }

    timers();
}
