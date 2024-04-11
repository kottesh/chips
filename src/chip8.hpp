#include <cstdint>

#ifndef CPU_CHIP_8
#define CPU_CHIP_8

class Chip8 {
private:
    uint8_t v[16]; // register from V0 to VF

    uint8_t mem[4096]; //memory
    uint16_t index; //index register

    uint16_t stack[16]; //stack
    uint8_t sp; //stack pointer

    uint16_t pc; //program counter 

    // 60hz timers
    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t keys[16]; //keymap
    uint32_t graphics[64*32];
public:
    Chip8();
    bool loadRom(const std::string& rom_file); // to load the ROM in the memory.

    // this for the stack operations
    void push(const uint16_t data);
    uint16_t pop();

    void machineCycle(); // to fetch, decode and execute
    void timers();
};

#endif

