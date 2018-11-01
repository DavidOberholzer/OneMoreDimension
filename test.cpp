#include <math.h>
#include <iostream>
#include "worldmath.h"
#include "data_io.h"

using namespace std;

int main()
{
	float zBuffer[WIDTH * HEIGHT];
	for (int i = 0; i < (WIDTH * HEIGHT); i++)
	{
		zBuffer[i] = 1.0;
	}
	Point3D triangle[3] = {
		Point3D(400, 250, 0),
		Point3D(390, 265, 0),
		Point3D(395, 275, 0)};
	bool orientation = triangleOrientation(triangle);
	float u[3] = {0.0, 0.0, 1.0};
	float v[3] = {0.0, 1.0, 1.0};
	Gradients gradients = Gradients(triangle, u, v);
	Edge *one = new Edge(gradients, triangle[0], triangle[2], u[0], v[0]);
	Edge *two = new Edge(gradients, triangle[0], triangle[1], u[0], v[0]);
	Edge *three = new Edge(gradients, triangle[1], triangle[2], u[1], v[1]);
	cout << "long side uStep: " << one->uStep << endl;
	cout << "long side vStep: " << one->vStep << endl;
	cout << "short side uStep: " << two->uStep << endl;
	cout << "short side vStep: " << two->vStep << endl;
	cout << "short side bottom uStep: " << three->uStep << endl;
	cout << "short side bottom vStep: " << three->vStep << endl;
	cout << "gradient uXstep: " << gradients.uXStep << endl;
    cout << "gradient vXstep: " << gradients.vXStep << endl;
	scanEdge(gradients, one, two, orientation, zBuffer, -1);
	scanEdge(gradients, one, three, orientation, zBuffer, -1);
}