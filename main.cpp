#include <iostream>
#include <SDL2/SDL.h>
#include "graphics.h"
#include "worldmath.h"
#include "data_io.h"

using namespace std;

int main()
{
    bool done = false;
    graphicsStartup();
    LoadObject((char *)"cube.txt");
    Matrix *P = new Matrix(1, 10, M_PI * 5.0 / 12.0, 3.0 / 4.0);
    float angle = 0.0;
    while (!done)
    {
        graphicsFrameReady();
        for (int i = 0; i < numTriangles; i++)
        {
            Triangle3D *t = &triangles[i];
            Point3D movedPoints[3];
            for (int j = 0; j < 3; j++) {
                Point3D p = vertices[t->getPoint(j) - 1];
                p.rotateY(angle);
                p.rotateZ(angle);
                p.translate(0, 0, -2);
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
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    done = true;
                    break;
                }
            }
        }
        SDL_Delay(10);
    }
    graphicsShutdown();
    return 0;
}