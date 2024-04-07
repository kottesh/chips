#include <cstdint>

#ifndef CPU_CHIP_8
#define CPU_CHIP_8

class Chip8 {
private:
    uint8_t v[16]; // register from V0 to VF

    uint8_t mem[4096]; //memory
    uint16_t i; //index register

    uint16_t stack[16]; //stack
    uint8_t sp; //stack pointer

    uint16_t pc; //program counter 

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t keymap[16]; //keypad for input
    uint32_t video[64*32];
public:
    Chip8();
    void loadRom(std::string rom_file); // to load the ROM in the memory.

    // this for the stack operations
    void stackPush(uint16_t &data);
    uint16_t stackPop();

    void machine(); // to fetch, decode, execute
    void timers();
};

#endif

