#include <math.h>
#include <iostream>
#include "worldmath.h"
#include "data_io.h"

using namespace std;

int main()
{
	// Create empty matrix
	Matrix empty = Matrix();
	empty.printMatrix();
	// Create matrix
	float values[16];
	for (int i = 0; i < 16; i++)
		values[i] = 0;
	values[0] = 5;
	values[5] = 5;
	values[10] = 5;
	values[15] = 5;
	Matrix first = Matrix(values, 2);
	first.printMatrix();
	// Set 3x3 matrix
	first.set3D();
	first.printMatrix();
	// Set 4x4 matrix
	first.set4D();
	first.printMatrix();
	// Create a ModelMatrix
	Point3D *vertex = new Point3D(1, 5, 4);
	Matrix model = Matrix(vertex);
	model.printMatrix();
	free(vertex);
	// Create ProjectionMatrix
	Matrix projection = Matrix(1, 10, M_PI * 5.0 / 12.0, 16.0 / 9.0);
	projection.printMatrix();
	// Create Matrix
	for (int i = 0; i < 16; i++)
	{
		values[i] = i + 3;
	}
	Matrix second = Matrix(values, 4);
	second.printMatrix();
	// Create Matrix
	values[4] = 5.5;
	values[9] = 3;
	values[13] = 24;
	Matrix third = Matrix(values, 4);
	third.printMatrix();
	// Multiply the matrices
	Matrix fourth = second * third;
	fourth.printMatrix();
	// Multiply small matrices
	second.set3D();
	third.set3D();
	fourth = second * third;
	fourth.printMatrix();

	// Make point and rotate it
	Point3D point = Point3D(2.0, 3.0, 5.0);
	// Rotate X 90 degrees
	point.print();
	point.rotateX(M_PI / 2);
	point.print();
	// Rotate Z 45 degrees
	point.rotateZ(M_PI / 4);
	point.print();
	// Translate point
	point.translate(6.0, 4.5, 3.4);
	point.print();

	// Load Cube Object
	LoadObject((char *)"cube.txt");
	UnloadData();

	// Dot Product
	float dot = Point3D(1, 2, 3).dot(Point3D(1, 5, 7));
	cout << "Dot Product Result: " << dot << endl;

	// Cross Product
	Point3D crs = Point3D(4, 9, 3).cross(Point3D(-9, -2, 4));
	crs.print();

	// Rotate with quarternion
	Quarternion q0 = Quarternion(Point3D(0, 0, 1), 0);
	Quarternion q1 = Quarternion(Point3D(0, 1, 0), M_PI / 2);
	Quarternion q2 = Quarternion(Point3D(1, 0, 0), M_PI / 4);
	Quarternion q3 = q2 * q1 * q0;
	Point3D v1 = Point3D(1, 0, 0);
	Point3D r1 = q1 * v1;
	r1.print();
	Point3D r2 = q2 * v1;
	r2.print();
	Point3D r3 = q3 * v1;
	r3.print();

	// Compound Quarternion rotation
	Quarternion q4 = Quarternion(M_PI / 4, M_PI / 4, M_PI / 2);
	Point3D r4 = q4 * Point3D(-1, 1, -1);
	r4.print();

	Quarternion q5 = Quarternion(M_PI / 4, M_PI / 2, M_PI / 2);
	q5.print();

	Point3D triangle[3] = {
		Point3D(400, 250, 0),
		Point3D(300, 350, 0),
		Point3D(340, 425, 0)
	};
	int c[3] = {0, 1, 2};
	Gradients gradients = Gradients(triangle, c);
	Edge one = Edge(gradients, triangle[0], triangle[2], 0);
	cout << "c[0].R: " << colors[c[0]].R << endl;
	cout << "c[2].R: " << colors[c[2]].R << endl;
	cout << "xStep: " << one.xStep << endl;
	cout << "m_color.R" << one.m_color.R << endl;
	cout << "m_colorStep.R: " << one.m_colorStep.R << endl;
}