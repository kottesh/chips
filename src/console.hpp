#ifndef CHIPS_CONSOLE
#define CHIPS_CONSOLE

#include <SDL2/SDL.h>
#include <string>

#include "chip8.hpp" 

#define IPS 5  // INS -> Instruction Per Second
#define SCALE 10

class Console {
private:
    SDL_Window *win;
    SDL_Renderer *renderer;
public:
    Console(const std::string &win_title, int win_width, int win_height);
    ~Console();

    void updateWindow(const bool *graphics_buf);
    void beep();
    void run(Chip8 *chips);
};

#endif
