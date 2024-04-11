#include <iostream> 

#include "chip8.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::cout << "Starting CHIPS...." << std::endl;

    Chip8* cpu = new Chip8();

    if (!cpu->loadRom(argv[1])) {
        return 1; // rom load failed
    }

    for (int i = 0; i < 10; i++)
       cpu->machineCycle();

    delete cpu;
    return 0;
}
