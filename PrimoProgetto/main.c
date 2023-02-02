#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <direct.h>

#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT



#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    SDL_Log("SDL is active!");

    SDL_Window* window = SDL_CreateWindow("SDL is active!", 100, 100, 512, 512, 0);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    //SDL_Log(_getcwd(NULL, 0)); //shows the directory
    
    
    int width;
    int heigth;
    int channel;
    unsigned char* pixels = stbi_load("menacing.png", &width, &heigth, &channel, 4);
    if (!pixels) {
        SDL_Log("Unable to open image: %s", stbi__g_failure_reason);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, heigth);
    if (!texture) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_UpdateTexture(texture, NULL, pixels, width * 4);

    SDL_Vertex vertex[4];
    vertex[0].position.x = 50;
    vertex[0].position.y = 50;
    vertex[0].color.r = 255;
    vertex[0].color.g = 255;
    vertex[0].color.b = 255;
    vertex[0].color.a = 255;
    vertex[0].tex_coord.x = 0;
    vertex[0].tex_coord.y = 0;
    vertex[1].position.x = 200;
    vertex[1].position.y = 50;
    vertex[1].color.r = 255;
    vertex[1].color.g = 255;
    vertex[1].color.b = 255;
    vertex[1].color.a = 255;
    vertex[1].tex_coord.x = 1;
    vertex[1].tex_coord.y = 0;
    vertex[2].position.x = 50;
    vertex[2].position.y = 200;
    vertex[2].color.r = 255;
    vertex[2].color.g = 255;
    vertex[2].color.b = 255;
    vertex[2].color.a = 255;
    vertex[2].tex_coord.x = 0;
    vertex[2].tex_coord.y = 1;
    vertex[3].position.x = 200;
    vertex[3].position.y = 200;
    vertex[3].color.r = 255;
    vertex[3].color.g = 255;
    vertex[3].color.b = 255;
    vertex[3].color.a = 255;
    vertex[3].tex_coord.x = 1;
    vertex[3].tex_coord.y = 1;

    int index[6] = {0, 1, 2, 1, 3, 2};


    struct nk_context *ctx = nk_sdl_init(window, renderer);
    
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_RenderGeometry(renderer, texture, vertex, 4, index, 6);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}

#ifdef FOOBAR
    int WinMain(
        void* hInstance,
        void* hPrevInstance,
        char* lpCmdLine,
        int       nShowCmd
    ) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
            return 1;
        }
        SDL_Log("SDL is active!");

        getchar();
        SDL_Quit();
        return 0;
    }
#endif // FOOBAR