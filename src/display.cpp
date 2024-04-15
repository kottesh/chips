#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "display.hpp"

Console::Console(const std::string &win_title, int win_width, int win_height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL init failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    try {// c_str() method converts the string to char*
        if (!(win = SDL_CreateWindow(win_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                     win_width, win_height, SDL_WINDOW_SHOWN))
        ) {
            throw std::runtime_error("failed to create window");
        }
        
        if (!(
            renderer = SDL_CreateRenderer(win, -1, 0)
        )) {
            throw std::runtime_error("failed to create renderer");
        }

        SDL_RenderSetLogicalSize(renderer, win_width, win_height);

        if (!(
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32)
        )) {
            throw std::runtime_error("failed to create texture");
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(1);// terminate the process
    }
}

void Console::updateWindow(uint32_t const *buf) {
    uint32_t pixel_buf[2048];

    for (int idx = 0; idx < 2048; idx++) {
        uint32_t pixel = buf[idx];
        pixel_buf[idx] = (0x00FFFFFF * pixel) | 0xFF000000; // see this line
    }

    SDL_UpdateTexture(texture, nullptr, pixel_buf, sizeof(uint32_t)*64);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Console::handleSystem(Chip8 *chips) {
    auto *keys = chips->keys;
    while (true) {
        chips->machineCycle();
        SDL_Event event; 
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(EXIT_SUCCESS);
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE: exit(EXIT_SUCCESS);
                        case SDLK_1: keys[0x1] = 1; break;
                        case SDLK_2: keys[0x2] = 1; break;
                        case SDLK_3: keys[0x3] = 1; break;
                        case SDLK_4: keys[0xC] = 1; break;

                        case SDLK_q: keys[0x4] = 1; break;
                        case SDLK_w: keys[0x5] = 1; break;
                        case SDLK_e: keys[0x6] = 1; break;
                        case SDLK_r: keys[0xD] = 1; break;

                        case SDLK_a: keys[0x7] = 1; break;
                        case SDLK_s: keys[0x8] = 1; break;
                        case SDLK_d: keys[0x9] = 1; break;
                        case SDLK_f: keys[0xE] = 1; break;

                        case SDLK_z: keys[0xA] = 1; break;
                        case SDLK_x: keys[0x0] = 1; break;
                        case SDLK_c: keys[0xB] = 1; break;
                        case SDLK_v: keys[0xF] = 1; break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_1: keys[0x1] = 0; break;
                        case SDLK_2: keys[0x2] = 0; break;
                        case SDLK_3: keys[0x3] = 0; break;
                        case SDLK_4: keys[0xC] = 0; break;

                        case SDLK_q: keys[0x4] = 0; break;
                        case SDLK_w: keys[0x5] = 0; break;
                        case SDLK_e: keys[0x6] = 0; break;
                        case SDLK_r: keys[0xD] = 0; break;

                        case SDLK_a: keys[0x7] = 0; break;
                        case SDLK_s: keys[0x8] = 0; break;
                        case SDLK_d: keys[0x9] = 0; break;
                        case SDLK_f: keys[0xE] = 0; break;

                        case SDLK_z: keys[0xA] = 0; break;
                        case SDLK_x: keys[0x0] = 0; break;
                        case SDLK_c: keys[0xB] = 0; break;
                        case SDLK_v: keys[0xF] = 0; break;
                    }
                    break;
            }

        }
        if (chips->redraw) {
            chips->redraw = false;
            updateWindow(chips->graphics);
        }
        SDL_Delay(10);
    }
}

Console::~Console() {
    // clearing the resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
}
