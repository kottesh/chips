#include <SDL2/SDL.h>
#include <string>

#include "chip8.hpp" 

class Console {
private:
    SDL_Window *win;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
public:
    Console(const std::string &win_title, int win_width, int win_height);
    ~Console();

    void updateWindow(uint32_t const *buf);
    void handleSystem(Chip8 *chips);
};
