#include <iostream>
#include <SDL2/SDL.h>
#include "graphics.h"
#include "data_io.h"
#include "structures.h"

using namespace std;

// struct gradients gradients;

void graphicsStartup()
{
    cout << "Starting SDL Window!" << endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    window = SDL_CreateWindow("OneMoreDimension", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    SDL_ShowCursor(SDL_DISABLE);
}

void graphicsShutdown()
{
    cout << "Shutting down SDL Window!" << endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void graphicsFrameReady()
{
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
}

void graphicsFrameDraw()
{
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void graphicsDrawPoint(int x, int y, int R, int G, int B)
{
    SDL_SetRenderDrawColor(renderer, R, G, B, 0x00);
    SDL_RenderDrawPoint(renderer, x, y);
}
