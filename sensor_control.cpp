#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <unistd.h>

#include "blocking_reader.h"
#include "graphics.h"
#include "worldmath.h"
#include "data_io.h"

using namespace std;
using namespace boost;

Point3D U = Point3D(0, 1, 0, 0); // Camera True Up vector
Point3D R = Point3D(1, 0, 0, 0); // Camera True Right vector
Point3D D = Point3D(0, 0, 1, 0); // Camera True Directional vector
float dx = 0.0;					 // Player x position
float dy = 0.0;					 // Player y position
float dz = 0.0;					 // Player z position
Point3D oU = Point3D(0, 1, 0, 0);
Point3D oR = Point3D(1, 0, 0, 0);
Point3D oD = Point3D(0, 0, 1, 0);

float qx = 0.0;
float qy = 0.0;
float qz = 0.0;

int main()
{
	asio::io_service io;
	asio::serial_port port(io);

	port.open("/dev/ttyACM0");
	port.set_option(asio::serial_port_base::baud_rate(115200));

	char c;
	int calibration = 0;
	bool done = false;
	graphicsStartup();
	LoadObject((char *)"sensor_shape.txt");
	Matrix *P = new Matrix(1, 15, M_PI * 5.0 / 12.0, 3.0 / 4.0);
	time_t start, end;
	float zBuffer[WIDTH * HEIGHT];
	while (!done)
	{
		start = clock();
		blocking_reader reader(port, 500);

		string rsp;

		while (reader.read_char(c) && c != '\n')
		{
			rsp += c;
		}

		if (c != '\n')
		{
			cout << "Read Timed out!" << endl;
		}
		float tqx, tqy, tqz;
		if (rsp[0] != 'O')
		{
			sscanf(rsp.c_str(), "%f%f%f%d", &tqx, &tqy, &tqz, &calibration);
		}
		// cout << tqx << endl;

		if (calibration > 0)
		{
			qx = tqx * (M_PI / 180);
			qy = tqy * (M_PI / 180);
			qz = tqz * (M_PI / 180);
		}

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

		Quarternion qu = Quarternion(oU, -qx);
		Quarternion qr = Quarternion(oR, -qy);
		Quarternion qd = Quarternion(oD, -qz);
		Quarternion t = qd * qr * qu;
		t.normalize();
		objects[0].drawObject(t, 0, 0, -3, P, &V, zBuffer);
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
				case SDLK_ESCAPE:
					done = true;
					break;
				}
			}
		}
		end = clock();
		double diff = difftime(end, start);
		if (diff < 16667)
		{
			int wait = 16667 - diff;
			usleep(wait);
		}
	}
	port.close();
	graphicsShutdown();
	return 0;
}