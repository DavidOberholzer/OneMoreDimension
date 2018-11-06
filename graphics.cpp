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
    window = SDL_CreateWindow("OneMoreDimension", 0, 0, WIDTH, HEIGHT, 0);
    surface = SDL_GetWindowSurface(window);
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

void graphicsDrawStraightLine(int x1, int x2, float z1, float z2, int y, float u0, float v0, float ooz0, float dooz, float duoz, float dvoz, int tex, int cIndex, float zBuffer[WIDTH * HEIGHT])
{
    Color color = colors[cIndex];
    struct texture *texture;
    // if (tex > 0)
    // {
    //     texture = &textures[tex - 1];
    // }
    // else
    // {
    SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
    // }
    int startX, endX;
    float startZ, endZ;
    if (x1 < x2)
    {
        startX = x1;
        endX = x2;
        startZ = z1;
        endZ = z2;
    }
    else
    {
        startX = x2;
        endX = x1;
        startZ = z2;
        endZ = z1;
    }

    for (int x = startX; x <= endX; x++)
    {
        int dx = x - startX;
        float t = dx / (float)(endX - startX);
        float z = endZ * t + startZ * (1 - t);
        int index = y * WIDTH + x;
        if (zBuffer[index] > z)
        {
            zBuffer[index] = z;
            // if (tex > 0)
            // {
            //     float ooz = gradients.dOneOverZdX * dx + dooz + ooz0;
            //     float u = clampF(1.0, 0.0, ((gradients.dUOverZdX * dx + duoz) / ooz) + u0);
            //     float v = clampF(1.0, 0.0, ((gradients.dVOverZdX * dx + dvoz) / ooz) + v0);

            //     int wPix = (texture->width - 1) * u;
            //     int hPix = (texture->height - 1) * v;

            //     int index = hPix * texture->width + wPix;
            //     SDL_SetRenderDrawColor(renderer, texture->pixels[index].R, texture->pixels[index].G, texture->pixels[index].B, 0x00);
            // }
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

void graphicsDrawLine(int x1, int y1, int x2, int y2, int cIndex)
{
    Color color = colors[cIndex];
    SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
    bool steep = false;
    if (abs(x1 - x2) < abs(y1 - y2))
    {
        swap(x1, y1);
        swap(x2, y2);
        steep = true;
    }
    if (x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dError = fabs(dy) * 2;
    float error = 0;
    int y = y1;
    for (int x = x1; x <= x2; x++)
    {
        if (steep)
        {
            SDL_RenderDrawPoint(renderer, y, x);
        }
        else
        {
            SDL_RenderDrawPoint(renderer, x, y);
        }
        error += dError;
        if (error > dx)
        {
            y += y2 > y1 ? 1 : -1;
            error -= dx * 2;
        }
    }
}