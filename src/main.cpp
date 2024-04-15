#include <iostream> 
#include <SDL2/SDL.h>

#include "chip8.hpp"
#include "display.hpp"

#define OK 1
#define FAIL 0

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/rom_file" << std::endl;
        return 1;
    }
    
    Console* console = new Console("Chips - a chip-8 interpreter", 640, 320);
    Chip8* cpu = new Chip8();
    if(!cpu->loadRom(argv[1])) {
        std::cerr << "failed to load rom file" << std::endl;
        return FAIL;
    } else {
        std::cout << "rom file loaded." << std::endl;
    }

    console->handleSystem(cpu);

    delete console;
    delete cpu;
    return OK;
}