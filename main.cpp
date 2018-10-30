#include <iostream>
#include <SDL2/SDL.h>
#include <time.h>
#include <unistd.h>

#include "graphics.h"
#include "worldmath.h"
#include "data_io.h"

using namespace std;

bool keysPressed[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Point3D U = Point3D(0, 1, 0);
Point3D R = Point3D(1, 0, 0);
Point3D D = Point3D(0, 0, 1);
float dx = 0.0;
float dy = 0.0;
float dz = 0.0;
float speed = 0.05;
Quarternion t;

void cameraMovement()
{
    Quarternion u, r, d;
    bool ub = false, rb = false, db = false;
    Point3D ws = D.scaledVector(speed);
    Point3D ad = R.scaledVector(speed);
    if (keysPressed[0])
    {
        dx -= ws.getX();
        dy -= ws.getY();
        dz -= ws.getZ();
    }
    if (keysPressed[1])
    {
        dx += ws.getX();
        dy += ws.getY();
        dz += ws.getZ();
    }
    if (keysPressed[2])
    {
        dx -= ad.getX();
        dy -= ad.getY();
        dz -= ad.getZ();
    }
    if (keysPressed[3])
    {
        dx += ad.getX();
        dy += ad.getY();
        dz += ad.getZ();
    }
    if (keysPressed[4])
    {
        u = Quarternion(U, 0.02);
    }
    if (keysPressed[5])
    {
        u = Quarternion(U, -0.02);
    }
    if (keysPressed[6])
    {
        r = Quarternion(R, -0.02);
    }
    if (keysPressed[7])
    {
        r = Quarternion(R, 0.02);
    }
    if (keysPressed[8])
    {
        d = Quarternion(D, -0.02);
    }
    if (keysPressed[9])
    {
        d = Quarternion(D, 0.02);
    }

    t = d * r * u;
    U = t * U;
    R = t * R;
    D = t * D;
}

int main()
{
    bool done = false;
    graphicsStartup();
    LoadObject((char *)"cube.txt");
    Matrix *P = new Matrix(1, 10, M_PI * 5.0 / 12.0, 3.0 / 4.0);
    time_t start, end;
    float angle = 0.0;
    float zBuffer[WIDTH * HEIGHT];
    while (!done)
    {
        start = clock();
        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                int index = i * WIDTH + j;
                zBuffer[index] = 1.0;
            }
        }
        graphicsFrameReady();
        Matrix V = viewMatrix(U, R, D, dx, dy, dz);
        objects[0].drawObject(0, 0, 0, 0, 0, -3, P, &V, zBuffer);
        // objects[0].drawObject(-M_PI /4, M_PI/4, angle, 1, 0, -3, P, &V, zBuffer);
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
                bool eType = event.type == SDL_KEYDOWN;
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    keysPressed[0] = eType;
                    break;
                case SDLK_s:
                    keysPressed[1] = eType;
                    break;
                case SDLK_a:
                    keysPressed[2] = eType;
                    break;
                case SDLK_d:
                    keysPressed[3] = eType;
                    break;
                case SDLK_g:
                    keysPressed[4] = eType;
                    break;
                case SDLK_j:
                    keysPressed[5] = eType;
                    break;
                case SDLK_y:
                    keysPressed[6] = eType;
                    break;
                case SDLK_h:
                    keysPressed[7] = eType;
                    break;
                case SDLK_t:
                    keysPressed[8] = eType;
                    break;
                case SDLK_u:
                    keysPressed[9] = eType;
                    break;
                case SDLK_ESCAPE:
                    done = true;
                    break;
                }
            }
        }
        cameraMovement();
        end = clock();
        double diff = difftime(end, start);
        if (diff < 16667)
        {
            int wait = 16667 - diff;
            usleep(wait);
        }
    }
    graphicsShutdown();
    return 0;
}