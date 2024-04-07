#include <iostream> 
#include <fstream> // ifstream, ofstream
#include <cstdint>

#include "cpu.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::cout << "Starting CHIPS...." << std::endl;

    Chip8* cpu = new Chip8();
    cpu->loadRom(argv[1]);
    int i = 0;
    while (i != 10) {
        cpu->machine();
        i++;
    }

    delete cpu;
    return 0;
}
