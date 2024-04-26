#include <iostream>
#include <cstdlib>

#include "console.hpp"

Console::Console(const std::string &win_title, const int win_width, const int win_height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL Init failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }           

    win = SDL_CreateWindow(win_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height, 0);
    if (!win) {
        std::cerr << "window creation failed." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cout << "renderer creation failed." << std::endl;
        SDL_Quit();
        std::exit(EXIT_FAILURE);
    }
}

void Console::updateWindow(const bool* graphics_buf) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for (int idx = 0; idx < 2048; idx++) {
        if (graphics_buf[idx]) {
            auto x = (idx % SCREEN_WIDTH) * SCALE;
            auto y = (idx / SCREEN_WIDTH) * SCALE;
            
            SDL_Rect rect = {x, y, 10, 10};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

void Console::run(Chip8 *chips) {
    bool *keys = chips->keyPad(); SDL_Event event; 
    bool running = true;
    while (running) {
        bool hold = false;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE: exit(EXIT_SUCCESS);

                        case SDLK_1: keys[0x1] = true; break;
                        case SDLK_2: keys[0x2] = true; break;
                        case SDLK_3: keys[0x3] = true; break;
                        case SDLK_4: keys[0xC] = true; break;

                        case SDLK_q: keys[0x4] = true; break;
                        case SDLK_w: keys[0x5] = true; break;
                        case SDLK_e: keys[0x6] = true; break;
                        case SDLK_r: keys[0xD] = true; break;

                        case SDLK_a: keys[0x7] = true; break;
                        case SDLK_s: keys[0x8] = true; break;
                        case SDLK_d: keys[0x9] = true; break;
                        case SDLK_f: keys[0xE] = true; break;

                        case SDLK_z: keys[0xA] = true; break;
                        case SDLK_x: keys[0x0] = true; break;
                        case SDLK_c: keys[0xB] = true; break;
                        case SDLK_v: keys[0xF] = true; break;

                        default: break;
                    }
                    hold = true;
                    if (hold) chips->cycle();
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_1: keys[0x1] = false; break;
                        case SDLK_2: keys[0x2] = false; break;
                        case SDLK_3: keys[0x3] = false; break;
                        case SDLK_4: keys[0xC] = false; break;

                        case SDLK_q: keys[0x4] = false; break;
                        case SDLK_w: keys[0x5] = false; break;
                        case SDLK_e: keys[0x6] = false; break;
                        case SDLK_r: keys[0xD] = false; break;

                        case SDLK_a: keys[0x7] = false; break;
                        case SDLK_s: keys[0x8] = false; break;
                        case SDLK_d: keys[0x9] = false; break;
                        case SDLK_f: keys[0xE] = false; break;

                        case SDLK_z: keys[0xA] = false; break;
                        case SDLK_x: keys[0x0] = false; break;
                        case SDLK_c: keys[0xB] = false; break;
                        case SDLK_v: keys[0xF] = false; break;
                        default: break;
                    }
                    hold = false;
                    break;
                default:
                    break;
            }
        } 
        if (hold) {
            continue;
        }

        for (int i = 0; i <= IPS; i++) {
            chips->cycle();
        }
        updateWindow(chips->getGraphics());
    }
}

void Console::beep() {
    // TODO: implement audio 
}

Console::~Console() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
