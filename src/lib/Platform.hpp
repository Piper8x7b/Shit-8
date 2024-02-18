//
// Created by Piper8x7b on 2/17/24.
//

#include <cstdint>
#include <SDL2/SDL.h>
#include <glad/glad.h>

class Platform {
public:
    Platform(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

    ~Platform();

    // Method to update the render texture with a buffer and pitch
    void Update(void const *buffer, int pitch);

    // Method to process input keys. It returns a boolean value indicating if the user wants to quit or not
    bool ProcessInput(uint8_t *keys);

private:
    SDL_Window *window{};
    SDL_GLContext gl_context{};
    GLuint framebuffer_texture;
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};
};
