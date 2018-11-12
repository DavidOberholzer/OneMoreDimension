#include <iostream>
#include <SDL2/SDL.h>
#include <time.h>
#include <unistd.h>

#include "graphics.h"
#include "worldmath.h"
#include "data_io.h"

using namespace std;

#define GRAVITY_ACCELERTATION 0.02
#define GRAVITY_TERMINAL_VELOCITY 0.50
#define PLAYER_HEIGHT 3
#define MOVEMENT_SPEED 0.1
#define CHUNK 25
#define FACE 1.0
#define ROTATION_SPEED 0.04

class Block
{
  public:
    Object *object;
    int x, y, z;
    bool loaded;
    Block();
    Block(Object *, int, int, int);
    ~Block();
};

Block::Block()
{
    this->loaded = false;
}

Block::Block(Object *object, int x, int y, int z)
{
    this->object = object;
    this->x = x;
    this->y = y;
    this->z = z;
    this->loaded = true;
}

Block::~Block()
{
}

bool keysPressed[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Point3D U = Point3D(0, 1, 0, 0); // Camera True Up vector
Point3D R = Point3D(1, 0, 0, 0); // Camera True Right vector
Point3D D = Point3D(0, 0, 1, 0); // Camera True Directional vector
Point3D T = Point3D(0, 1, 0, 0); // Camera FPS Up vector
Point3D F = Point3D(0, 0, 1, 0); // Camera FPS Face vector
Point3D S = Point3D(1, 0, 0, 0); // Camera FPS Side vector
float dx = 0.0;                  // Player x position
float dy = 0.0;                  // Player y position
float dz = 0.0;                  // Player z position
float y_v = 0.0;                 // Player y velocity
Quarternion t;
bool flying = true;
bool standing = false;
float corners[8] = {
    0.5, 0.5,
    0.5, -0.5,
    -0.5, -0.5,
    -0.5, 0.5};

void handleCollision(float x, float y, float z, Block blocks[CHUNK])
{
    if (flying)
    {
        dx += x;
        dy += y;
        dz += z;
        return;
    }
    int py = (int)floor(dy + PLAYER_HEIGHT - 1);
    int ty = py - 2;
    for (int i = 0; i < CHUNK; i++)
    {
        bool blocking = false;
        if (blocks[i].y >= ty && blocks[i].y <= py && blocks[i].loaded)
        {
            for (int j = 0; j < 8; j += 2)
            {
                float x1 = blocks[i].x + corners[j];
                float z1 = blocks[i].z + corners[j + 1];
                int newJ = j + 2 > 7 ? 0 : j + 2;
                float x2 = blocks[i].x + corners[newJ];
                float z2 = blocks[i].z + corners[newJ + 1];
                float vx = dx + (x > 0 ? x + FACE : x - FACE);
                float vz = dz + (z > 0 ? z + FACE : z - FACE);
                bool behind = behindLine(vx, vz, x1, z1, x2, z2);
                int overlap = boxesOverlap(dx, dz, vx, vz, x1, z1, x2, z2);
                if (behind && overlap && !blocking)
                {
                    blocking = true;
                    float *newVector = vectorProjection(x, z, x2 - x1, z2 - z1);

                    x = *newVector;
                    z = *(newVector + 1);
                }
            }
        }
    }
    dx += x;
    dz += z;
}

void cameraMovement(Block blocks[CHUNK])
{
    Quarternion u, r, d;
    bool ub = false, rb = false, db = false;
    Point3D fws = D.scaledVector(MOVEMENT_SPEED);
    Point3D fad = R.scaledVector(MOVEMENT_SPEED);
    Point3D ws = F.scaledVector(MOVEMENT_SPEED);
    Point3D ad = S.scaledVector(MOVEMENT_SPEED);
    float x = 0, y = 0, z = 0;
    if (keysPressed[0])
    {
        if (flying)
        {
            x -= fws.getX();
            y -= fws.getY();
            z -= fws.getZ();
        }
        else
        {
            x -= ws.getX();
            y -= ws.getY();
            z -= ws.getZ();
        }
    }
    if (keysPressed[1])
    {
        if (flying)
        {
            x += fws.getX();
            y += fws.getY();
            z += fws.getZ();
        }
        else
        {
            x += ws.getX();
            y += ws.getY();
            z += ws.getZ();
        }
    }
    if (keysPressed[2])
    {
        if (flying)
        {
            x -= fad.getX();
            y -= fad.getY();
            z -= fad.getZ();
        }
        else
        {
            x -= ad.getX();
            y -= ad.getY();
            z -= ad.getZ();
        }
    }
    if (keysPressed[3])
    {
        if (flying)
        {
            x += fad.getX();
            y += fad.getY();
            z += fad.getZ();
        }
        else
        {
            x += ad.getX();
            y += ad.getY();
            z += ad.getZ();
        }
    }
    if (keysPressed[4])
    {
        u = Quarternion(flying ? U : T, ROTATION_SPEED);
    }
    if (keysPressed[5])
    {
        u = Quarternion(flying ? U : T, -ROTATION_SPEED);
    }
    if (keysPressed[6])
    {
        r = Quarternion(R, -ROTATION_SPEED);
    }
    if (keysPressed[7])
    {
        r = Quarternion(R, ROTATION_SPEED);
    }
    if (keysPressed[8] && flying)
    {
        d = Quarternion(D, -ROTATION_SPEED);
    }
    if (keysPressed[9] && flying)
    {
        d = Quarternion(D, ROTATION_SPEED);
    }

    t = d * r * u;
    U = t * U;
    R = t * R;
    D = t * D;
    if (!flying)
    {
        F = u * F;
        S = u * S;
    }
    handleCollision(x, y, z, blocks);
}

void onFloor(Block blocks[CHUNK])
{
    int py = (int)round(dy);
    standing = false;
    for (int i = 0; i < CHUNK; i++)
    {
        int next_y = py + PLAYER_HEIGHT;
        if (!blocks[i].loaded)
            continue;
        if (fabs(dx - blocks[i].x) > 0.75 || fabs(dz - blocks[i].z) > 0.75 || py > blocks[i].y)
            continue;
        if (next_y > blocks[i].y)
        {
            standing = true;
            break;
        }
    }
}

void accelerate(float *value, float max, float a)
{
    *value = (*value + a) > max ? max : (*value + a);
}

void falling(Block blocks[CHUNK])
{
    if (!standing && !flying)
    {
        accelerate(&y_v, GRAVITY_TERMINAL_VELOCITY, GRAVITY_ACCELERTATION);
    }
    else if (y_v > 0)
    {
        y_v = 0;
    }
    dy += y_v;
}

int main()
{
    bool done = false;
    graphicsStartup();
    LoadObject((char *)"grass_cube.txt");
    LoadObject((char *)"sand_cube.txt");
    LoadObject((char *)"water_cube.txt");
    Matrix *P = new Matrix(1, 15, M_PI * 5.0 / 12.0, 3.0 / 4.0);
    Block blocks[CHUNK] = {
        Block(&objects[0], 0, 3, -1),
        Block(&objects[0], -1, 3, -1),
        Block(&objects[0], 0, 3, 0),
        Block(&objects[0], 0, 3, -2),
        Block(&objects[0], 1, 3, 0),
        Block(&objects[0], 1, 3, -2),
        Block(&objects[0], 0, 4, -3),
        Block(&objects[0], 0, 4, -4),
        Block(&objects[0], 0, 5, -5),
        Block(&objects[1], 0, 5, -6),
        Block(&objects[1], 1, 5, -6),
        Block(&objects[1], -1, 5, -6),
        Block(&objects[1], 0, 5, -7),
        Block(&objects[1], 1, 5, -7),
        Block(&objects[1], -1, 5, -7),
        Block(&objects[1], 0, 5, -8),
        Block(&objects[1], 0, 5, -10),
        Block(&objects[1], 0, 5, -11),
        Block(&objects[1], 1, 5, -11),
        Block(&objects[1], -1, 5, -11),
        Block(&objects[0], 0, 4, -12),
        Block(&objects[0], 1, 4, -12),
        Block(&objects[0], -1, 4, -12)
        };
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
        for (int i = 0; i < CHUNK; i++)
        {
            if (!blocks[i].loaded)
                continue;
            blocks[i].object->drawObject(0, 0, 0, blocks[i].x, blocks[i].y, blocks[i].z, P, &V, zBuffer);
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
                case SDLK_f:
                    if (eType)
                    {
                        flying = !flying;
                        y_v = 0;
                        U = Point3D(0, 1, 0, 0);
                        R = S;
                        D = F;
                    }
                    break;
                case SDLK_r:
                    if (eType)
                    {
                        dx = 0.0;
                        dy = 0.0;
                        dz = 0.0;
                        U = Point3D(0, 1, 0, 0);
                        R = Point3D(1, 0, 0, 0);
                        D = Point3D(0, 0, 1, 0);
                        T = Point3D(0, 1, 0, 0);
                        F = Point3D(0, 0, 1, 0);
                        S = Point3D(1, 0, 0, 0);
                    }
                    break;
                case SDLK_SPACE:
                    if (eType && !flying && standing)
                    {
                        standing = false;
                        y_v -= 0.25;
                    }
                    break;
                case SDLK_ESCAPE:
                    done = true;
                    break;
                }
            }
        }
        cameraMovement(blocks);
        onFloor(blocks);
        falling(blocks);
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