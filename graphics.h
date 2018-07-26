#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#define WIDTH 640
#define HEIGHT 480

static SDL_Window *window;
static SDL_Surface *surface;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

struct Color {
	int R;
	int G;
	int B;
	int A;
};

static Color blue = {0x00, 0x00, 0xff, 0xff};
static Color green = {0x00, 0xff, 0x00, 0xff};
static Color red = {0xff, 0x00, 0x00, 0xff};
static Color yellow = {0xff, 0xff, 0x00, 0xff};
static Color white = {0xff, 0xff, 0xff, 0xff};

void graphicsStartup();
void graphicsShutdown();
void graphicsFrameReady();
void graphicsFrameDraw();
void graphicsDrawPoint(int, int, Color);
void graphicsDrawLine(int, int, int, int, Color);

#endif