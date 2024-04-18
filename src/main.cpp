#include <iostream> 
#include <SDL2/SDL.h>

#include "chip8.hpp"
#include "display.hpp"

#define OK 1
#define FAIL 0

#define SCALE 10
#define WINDOW_WIDTH 64 * SCALE
#define WINDOW_HEIGHT 32 * SCALE

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/rom_file" << std::endl;
        return 1;
    }
    
    Console* console = new Console("Chips - a chip-8 interpreter", WINDOW_WIDTH, WINDOW_HEIGHT);
    Chip8* cpu = new Chip8();

    if(!cpu->loadRom(argv[1])) {
        return FAIL;
    } else {
        std::cout << "rom file loaded." << std::endl;
    }

    console->handleSystem(cpu);

    delete console;
    delete cpu;
    return OK;
}
