#include <iostream>
#include <SDL2/SDL.h>
#include "graphics.h"
#include "worldmath.h"

using namespace std;

void rasterize(Vector3D *v, Color color, int yBuffer[])
{
    Point3D *p1 = v->getP1();
    Point3D *p2 = v->getP2();
    if (p2->getX() < p1->getX())
    {
        swap(p2, p1);
    }
    for (int i = p1->getX(); i <= p2->getX(); i++)
    {
        float xRatio = (i - p1->getX()) / (float)(p2->getX() - p1->getX());
        int y = p1->getY() * (1. - xRatio) + p2->getY() * xRatio;
        if (yBuffer[i] > y)
        {
            yBuffer[i] = y;
            graphicsDrawPoint(i, 239, color);
            graphicsDrawPoint(i, 240, color);
            graphicsDrawPoint(i, 241, color);
        }
    }
}

int main()
{
    bool done = false;
    Point3D points[3] = {Point3D(10, 70, 0), Point3D(50, 160, 0), Point3D(70, 80, 0)};
    Point3D points2[3] = {Point3D(180, 50, 0), Point3D(150, 1, 0), Point3D(70, 180, 0)};
    Point3D points3[3] = {Point3D(180, 150, 0), Point3D(120, 160, 0), Point3D(130, 180, 0)};

    Triangle3D *triangle1 = new Triangle3D(&points[0], &points[1], &points[2]);
    Triangle3D *triangle2 = new Triangle3D(&points2[0], &points2[1], &points2[2]);
    Triangle3D *triangle3 = new Triangle3D(&points3[0], &points3[1], &points3[2]);

    Vector3D *v1 = new Vector3D(-40, -20, 2, 20, -20, 2);
    Vector3D *v2 = new Vector3D(20, -20, 2, 20, 20, 2);
    Vector3D *v3 = new Vector3D(20, 20, 2, -20, 20, 2);
    Vector3D *v4 = new Vector3D(-20, 20, 2, -40, -20, 2);
    Vector3D *v5 = new Vector3D(-40, -20, 2, -40, -20, 4);
    Vector3D *v6 = new Vector3D(20, -20, 2, 20, -20, 4);
    Vector3D *v7 = new Vector3D(20, 20, 2, 20, 20, 4);
    Vector3D *v8 = new Vector3D(-20, 20, 2, -20, 20, 4);
    Vector3D *v9 = new Vector3D(-40, -20, 4, 20, -20, 4);
    Vector3D *v10 = new Vector3D(20, -20, 4, 20, 20, 4);
    Vector3D *v11 = new Vector3D(20, 20, 4, -20, 20, 4);
    Vector3D *v12 = new Vector3D(-20, 20, 4, -40, -20, 4);
    Vector3D *v13 = new Vector3D(-320, -240, 2, 320, 240, 2);

    graphicsStartup();
    while (!done)
    {
        graphicsFrameReady();
        // triangle1->draw(green);
        // triangle2->draw(blue);
        // triangle3->draw(red);
        v1->draw(green);
        v2->draw(green);
        v3->draw(green);
        v4->draw(green);
        v5->draw(red);
        v6->draw(red);
        v7->draw(red);
        v8->draw(red);
        v9->draw(blue);
        v10->draw(blue);
        v11->draw(blue);
        v12->draw(blue);
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
    delete triangle1;
    delete triangle2;
    delete triangle3;
    graphicsShutdown();
    return 0;
}