#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#define WIDTH 640
#define WIDTHD2 320
#define HEIGHT 480
#define HEIGHTD2 240
#include "worldmath.h"
#include "structures.h"

static SDL_Window *window;
static SDL_Surface *surface;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

extern struct gradients gradients;

struct Color
{
	int R;
	int G;
	int B;
	int A;
};

static Color colors[6] = {
	(struct Color){0x00, 0x00, 0xff, 0xff},
	(struct Color){0x00, 0xff, 0x00, 0xff},
	(struct Color){0xff, 0x00, 0x00, 0xff},
	(struct Color){0xff, 0xff, 0x00, 0xff},
	(struct Color){0xff, 0x00, 0xff, 0xff},
	(struct Color){0xff, 0xff, 0xff, 0xff}};

void graphicsStartup();
void graphicsShutdown();
void graphicsFrameReady();
void graphicsFrameDraw();
void graphicsDrawPoint(int, int, int);
void graphicsDrawStraightLine(int, int, float, float, int, float, float, float, float, float, float, int, int, float[WIDTH * HEIGHT]);
void graphicsDrawLine(int, int, int, int, int);

#endif