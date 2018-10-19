#include <iostream>
#include <SDL2/SDL.h>
#include "graphics.h"
#include "worldmath.h"
#include "data_io.h"

using namespace std;

float zR = 0.0;
float xR = 0.0;
float yR = 0.0;
bool keysPressed[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void movement()
{
    if (keysPressed[4])
    {
        zR -= 0.02;
    }
    if (keysPressed[5])
    {
        zR += 0.02;
    }
    if (keysPressed[6])
    {
        xR -= 0.02;
    }
    if (keysPressed[7])
    {
        xR += 0.02;
    }
    if (keysPressed[8])
    {
        yR -= 0.02;
    }
    if (keysPressed[9])
    {
        yR += 0.02;
    }
}

int main()
{
    bool done = false;
    graphicsStartup();
    LoadObject((char *)"cube.txt");
    Matrix *P = new Matrix(1, 10, M_PI * 5.0 / 12.0, 3.0 / 4.0);
    Point3D origin = Point3D(0, 0, 0);
    Point3D U = Point3D(0, -1, 0);
    Point3D R = Point3D(1, 0, 0);
    Point3D D = Point3D(0, 0, 1);
    float angle = 0.0;
    while (!done)
    {
        graphicsFrameReady();
        for (int i = 0; i < numTriangles; i++)
        {
            Triangle3D *t = &triangles[i];
            Point3D movedPoints[3];
            for (int j = 0; j < 3; j++)
            {
                Point3D p = vertices[t->getPoint(j) - 1];
                p.rotateQ(M_PI / 4, M_PI / 2, angle);
                p.translate(0, 0, -3);
                movedPoints[j] = p;
            }
            drawTriangle(blue, P, movedPoints);
        }
        angle += 0.01;
        graphicsFrameDraw();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    keysPressed[0] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_s:
                    keysPressed[1] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_a:
                    keysPressed[2] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_d:
                    keysPressed[3] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_g:
                    keysPressed[4] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_j:
                    keysPressed[5] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_y:
                    keysPressed[6] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_h:
                    keysPressed[7] = event.type == SDL_KEYDOWN ? true : false;
                    break;
                case SDLK_ESCAPE:
                    done = true;
                    break;
                }
            }
        }
        movement();
        SDL_Delay(10);
    }
    graphicsShutdown();
    return 0;
}