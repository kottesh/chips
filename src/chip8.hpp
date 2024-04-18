#include <cstdint>

#ifndef CPU_CHIP_8
#define CPU_CHIP_8

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define NUM_REGS 16
#define NUM_KEYS 16
#define STACK_SIZE 16
#define MEM_SIZE 4096 
#define FONT_SIZE 80
#define START_ADDR 0x200

class Chip8 {
private:
    uint8_t v[NUM_REGS]; // register from V0 to VF uint8_t mem[MEM_SIZE]; //memory uint16_t index; //index register uint16_t stack[STACK_SIZE]; //stack
    uint8_t mem[MEM_SIZE]; // memory 
    uint16_t stack[STACK_SIZE];
    bool graphics[SCREEN_WIDTH * SCREEN_HEIGHT]; // pixel data
    bool keys[NUM_KEYS]; //keymap
    uint16_t index; // index reg
    uint8_t sp; //stack pointer
    uint16_t pc; //program counter 
    uint8_t delay_timer;
    uint8_t sound_timer;

    const uint8_t fonts[FONT_SIZE] = {
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
public:
    Chip8();
    void reset();
    bool loadRom(const std::string& rom_file); // to load the ROM in the memory.
    void push(const uint16_t data);
    uint16_t pop();
    void cycle(); // to fetch, decode and execute
    void timers();
    bool* keyPad();
    bool* getGraphics();
};

#endif
