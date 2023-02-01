#define SDL_MAIN_HANDLED
#include <stdbool.h>
#include <stdio.h>
#include "SDL.h"

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    SDL_Log("SDL is active!");

    SDL_Window* window = SDL_CreateWindow("SDL is active!", 100, 100, 512, 512, 0);
    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }


    // Game loop
    int running = 1;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
        }
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

#ifdef DEAD
int WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nShowCmd)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    SDL_Log("SDL is active!");


    SDL_Quit();
    return 0;
}
#endif