#include <SDL2/SDL.h>
#include <string>

#include "chip8.hpp" 

class Console {
private:
    SDL_Window *win;
    SDL_Renderer *renderer;
public:
    Console(const std::string &win_title, int win_width, int win_height);
    ~Console();

    void updateWindow(const bool *graphics_buf);
    void handleSystem(Chip8 *chips);
};
