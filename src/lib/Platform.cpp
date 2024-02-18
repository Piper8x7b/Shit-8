//
// Created by Piper8x7b on 2/17/24.
//

#include "Platform.hpp"
#include "Chip8.hpp"
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>


Platform::Platform(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    // Initialize SDL video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create an SDL window
    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    // Create an SDL renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Create an SDL texture target
    texture = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}


Platform::~Platform() {
    // Clean up SDL resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Update the texture and render it
void Platform::Update(void const *buffer, int pitch) {
    // Update the texture with new pixel data
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);

    // Clear the current rendering target
    SDL_RenderClear(renderer);

    // Copy the texture to the render target
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Update the screen with the render target
    SDL_RenderPresent(renderer);
}


// Process user input
bool Platform::ProcessInput(uint8_t *keys) {
    // Flag to indicate if the user wants to quit
    bool quit = false;

    // SDL event to handle
    SDL_Event event;

    // Poll for currently pending SDL events
    while (SDL_PollEvent(&event)) {
        // Hadle the different types of events
        switch (event.type) {
            case SDL_QUIT: {
                quit = true;
            }
                break;

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: {
                        quit = true;
                    }
                        break;

                    case SDLK_x: {
                        keys[0] = 1;
                    }
                        break;

                    case SDLK_1: {
                        keys[1] = 1;
                    }
                        break;

                    case SDLK_2: {
                        keys[2] = 1;
                    }
                        break;

                    case SDLK_3: {
                        keys[3] = 1;
                    }
                        break;

                    case SDLK_q: {
                        keys[4] = 1;
                    }
                        break;

                    case SDLK_w: {
                        keys[5] = 1;
                    }
                        break;

                    case SDLK_e: {
                        keys[6] = 1;
                    }
                        break;

                    case SDLK_a: {
                        keys[7] = 1;
                    }
                        break;

                    case SDLK_s: {
                        keys[8] = 1;
                    }
                        break;

                    case SDLK_d: {
                        keys[9] = 1;
                    }
                        break;

                    case SDLK_z: {
                        keys[0xA] = 1;
                    }
                        break;

                    case SDLK_c: {
                        keys[0xB] = 1;
                    }
                        break;

                    case SDLK_4: {
                        keys[0xC] = 1;
                    }
                        break;

                    case SDLK_r: {
                        keys[0xD] = 1;
                    }
                        break;

                    case SDLK_f: {
                        keys[0xE] = 1;
                    }
                        break;

                    case SDLK_v: {
                        keys[0xF] = 1;
                    }
                        break;
                }
            }
                break;

            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                    case SDLK_x: {
                        keys[0] = 0;
                    }
                        break;

                    case SDLK_1: {
                        keys[1] = 0;
                    }
                        break;

                    case SDLK_2: {
                        keys[2] = 0;
                    }
                        break;

                    case SDLK_3: {
                        keys[3] = 0;
                    }
                        break;

                    case SDLK_q: {
                        keys[4] = 0;
                    }
                        break;

                    case SDLK_w: {
                        keys[5] = 0;
                    }
                        break;

                    case SDLK_e: {
                        keys[6] = 0;
                    }
                        break;

                    case SDLK_a: {
                        keys[7] = 0;
                    }
                        break;

                    case SDLK_s: {
                        keys[8] = 0;
                    }
                        break;

                    case SDLK_d: {
                        keys[9] = 0;
                    }
                        break;

                    case SDLK_z: {
                        keys[0xA] = 0;
                    }
                        break;

                    case SDLK_c: {
                        keys[0xB] = 0;
                    }
                        break;

                    case SDLK_4: {
                        keys[0xC] = 0;
                    }
                        break;

                    case SDLK_r: {
                        keys[0xD] = 0;
                    }
                        break;

                    case SDLK_f: {
                        keys[0xE] = 0;
                    }
                        break;

                    case SDLK_v: {
                        keys[0xF] = 0;
                    }
                        break;
                }
            }
                break;
        }
    }

    // Return the quit flag
    return quit;
}
