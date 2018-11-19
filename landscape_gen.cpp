#include <iostream>
#include <cstdlib>
#include <math.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <unistd.h>

#include "graphics.h"
#include "worldmath.h"

using namespace std;

#define CHANGE 1
#define SIZE 289
#define ROTATION_SPEED 0.04
#define MOVEMENT_SPEED 0.10

static Object landscape;

bool keysPressed[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Point3D U = Point3D(0, 1, 0, 0);						   // Camera True Up vector
Point3D R = Point3D(-sin(M_PI / 4), 0, sin(M_PI / 4), 0);  // Camera True Right vector
Point3D D = Point3D(-sin(M_PI / 4), 0, -sin(M_PI / 4), 0); // Camera True Directional vector
float dx = -4.0;										   // Player x position
float dy = -5.0;										   // Player y position
float dz = -4.0;										   // Player z position
Quarternion t;

void cameraMovement()
{
	Quarternion u, r, d;
	bool ub = false, rb = false, db = false;
	Point3D ws = D.scaledVector(MOVEMENT_SPEED);
	Point3D ad = R.scaledVector(MOVEMENT_SPEED);
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
		u = Quarternion(U, ROTATION_SPEED);
	}
	if (keysPressed[5])
	{
		u = Quarternion(U, -ROTATION_SPEED);
	}
	if (keysPressed[6])
	{
		r = Quarternion(R, -ROTATION_SPEED);
	}
	if (keysPressed[7])
	{
		r = Quarternion(R, ROTATION_SPEED);
	}
	if (keysPressed[8])
	{
		d = Quarternion(D, -ROTATION_SPEED);
	}
	if (keysPressed[9])
	{
		d = Quarternion(D, ROTATION_SPEED);
	}

	t = d * r * u;
	U = t * U;
	R = t * R;
	D = t * D;
}

int clamp(int max, int min, int value)
{
	return value < max ? (value > min ? value : min) : max;
}

int lerpColorComponent(int max, int min, int height, int start, int end)
{
	int value = clamp(max, min, height);
	float t = (value - min) / (max - min);
	return end * t + start * (1 - t);
}

Color getHeightColor(int array[], int index)
{
	return Color(
		lerpColorComponent(3, -5, array[index], 0xff, 0x00),
		lerpColorComponent(2, -2, array[index], 0x00, 0xff),
		lerpColorComponent(5, -3, array[index], 0x00, 0xff),
		0x00);
}

int variance()
{
	// Give a random number between -CHANGE/2 and +CHANGE/2
	return rand() % (CHANGE + 1) - (CHANGE / 2);
}

void squareStep(int array[], int step, int size, int *nextStep)
{
	*nextStep = size;
	for (int y = 0; y < (size - 1); y += step)
	{
		int y_max = y + step;
		int y_avg = (y + y + step);
		int y_mid = y_avg / 2;
		if (y_mid < *nextStep)
			*nextStep = y_mid;
		for (int x = 0; x < (size - 1); x += step)
		{
			int x_max = x + step;
			int x_avg = (x + x + step);
			int x_mid = x_avg / 2;
			int new_index = y_mid * size + x_mid;
			int index0 = y * size + x;
			int index1 = y * size + x_max;
			int index2 = y_max * size + x;
			int index3 = y_max * size + x_max;
			array[new_index] = ((array[index0] + array[index1] + array[index2] + array[index3]) / 4) + variance();
		}
	}
}

void diamondStep(int array[], int step, int size)
{
	int true_step = 2 * step;
	for (int y = step; y < size; y += true_step)
	{
		int y_top = y - step * 2;
		int y_top_mid = y - step;
		int y_bottom = y + step * 2;
		int y_bottom_mid = y + step;
		bool top = y_top < 0;
		bool bottom = y_bottom > (size - 1);
		for (int x = step; x < size; x += true_step)
		{
			int x_left = x - step * 2;
			int x_left_mid = x - step;
			int x_right = x + step * 2;
			int x_right_mid = x + step;
			bool left = x_left < 0;
			bool right = x_right > (size - 1);

			// Center point
			int index0 = y * size + x;

			// Get New Left Point
			int new_index = y * size + x_left_mid;
			int index1 = y_top_mid * size + x_left_mid;
			int index2 = y_bottom_mid * size + x_left_mid;
			int n = 3;
			int value = array[index0] + array[index1] + array[index2];
			if (!left)
			{
				int index3 = y * size + x_left;
				n = 4;
				value += array[index3];
			}
			array[new_index] = (value / n) + variance();

			// Get New Top Point
			new_index = y_top_mid * size + x;
			index1 = y_top_mid * size + x_left_mid;
			index2 = y_top_mid * size + x_right_mid;
			n = 3;
			value = array[index0] + array[index1] + array[index2];
			if (!top)
			{
				int index3 = y_top * size + x;
				n = 4;
				value += array[index3];
			}
			array[new_index] = (value / n) + variance();

			if (right)
			{
				// Get New Right point if end of the line
				new_index = y * size + x_right_mid;
				index1 = y_top_mid * size + x_right_mid;
				index2 = y_bottom_mid * size + x_right_mid;
				array[new_index] = ((array[index0] + array[index1] + array[index2]) / 3) + variance();
			}

			if (bottom)
			{
				// Get New Bottom Point if bottom of array
				new_index = y_bottom_mid * size + x;
				index1 = y_bottom_mid * size + x_left_mid;
				index2 = y_bottom_mid * size + x_right_mid;
				array[new_index] = ((array[index0] + array[index1] + array[index2]) / 3) + variance();
			}
		}
	}
}

void printArray(int array[], int length)
{
	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < length; x++)
		{
			int index = y * length + x;
			cout << array[index] << " ";
		}
		cout << endl;
	}
}

void generateMap(int array[], int size, int h0, int h1, int h2, int h3)
{
	int value = size - 1;

	// Check for invalid array size
	int n = 0;
	while (value != 1)
	{
		if (value % 2 != 0)
		{
			cout << "Invalid Array Size. Valid size is should be of the form '2^n - 1', where 'n' in a whole number." << endl;
			exit(1);
		}
		value = value / 2;
		n++;
	}
	int index0 = 0;
	int index1 = size - 1;
	int index2 = (size - 1) * size;
	int index3 = (size * size) - 1;
	array[index0] = h0;
	array[index1] = h1;
	array[index2] = h2;
	array[index3] = h3;
	int steps = n * 2;
	int step = size - 1;
	for (int i = 1; i <= steps; i++)
	{
		if (i % 2 == 0)
		{
			diamondStep(array, step, size);
		}
		else
		{
			squareStep(array, step, size, &step);
		}
	}
}

void initializeLandscape(int h0, int h1, int h2, int h3)
{
	int array[SIZE];
	int length = sqrt(SIZE);
	int triangle_limit = sqrt(SIZE) - 1;

	for (int i = 0; i < SIZE; i++)
		array[i] = 0;

	generateMap(array, length, h0, h1, h2, h3);
	printArray(array, length);
	landscape.numVertices = SIZE;
	landscape.vertices = (Point3D *)malloc(landscape.numVertices * sizeof(*landscape.vertices));
	landscape.numTriangles = (length - 1) * (length - 1) * 2;
	landscape.triangles = (Triangle3D *)malloc(landscape.numTriangles * sizeof(*landscape.triangles));
	for (int z = 0; z < length; z++)
	{
		// cout << "z: " << z << endl;
		for (int x = 0; x < length; x++)
		{
			int index0 = z * length + x;
			// cout << "x: " << x << endl;
			landscape.vertices[index0].setX(x);
			landscape.vertices[index0].setZ(z);
			landscape.vertices[index0].setY(array[index0]);
			if (x < triangle_limit && z < triangle_limit)
			{
				int triangle_index = z * (triangle_limit * 2) + x * 2;
				int index1 = (z + 1) * length + x;
				int index2 = (z + 1) * length + (x + 1);
				int index3 = z * length + (x + 1);

				Color c0 = getHeightColor(array, index0);
				Color c1 = getHeightColor(array, index1);
				Color c2 = getHeightColor(array, index2);
				Color c3 = getHeightColor(array, index3);

				landscape.triangles[triangle_index].setP1(index0 + 1);
				landscape.triangles[triangle_index].setP2(index1 + 1);
				landscape.triangles[triangle_index].setP3(index2 + 1);
				landscape.triangles[triangle_index].setTexture(-1);

				landscape.triangles[triangle_index].colors[0] = c0;
				landscape.triangles[triangle_index].colors[1] = c1;
				landscape.triangles[triangle_index].colors[2] = c2;

				landscape.triangles[triangle_index + 1].setP1(index0 + 1);
				landscape.triangles[triangle_index + 1].setP2(index3 + 1);
				landscape.triangles[triangle_index + 1].setP3(index2 + 1);
				landscape.triangles[triangle_index + 1].setTexture(-1);

				landscape.triangles[triangle_index + 1].colors[0] = c0;
				landscape.triangles[triangle_index + 1].colors[1] = c3;
				landscape.triangles[triangle_index + 1].colors[2] = c2;
			}
		}
	}
}

int main()
{
	Quarternion q = Quarternion(R, 0.5);
	U = q * U;
	initializeLandscape(7, 4, 3, -10);
	bool done = false;
	Matrix *P = new Matrix(1, 60, M_PI * 5.0 / 12.0, 3.0 / 4.0);
	graphicsStartup();
	time_t start, end;
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
		// Draw objects
		Matrix V = viewMatrix(U, R, D, dx, dy, dz);
		landscape.drawObject(0, 0, 0, 0, 0, 0, P, &V, zBuffer);
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
	delete P;
	return 0;
}