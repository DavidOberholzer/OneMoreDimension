#ifndef WORLDMATH_H_INCLUDED
#define WORLDMATH_H_INCLUDED
#include "SDL2/SDL.h"
#include "graphics.h"
#include "structures.h"

class Color
{
  public:
	float R, G, B, A;
	Color();
	Color(float, float, float, float);
	Color operator+(Color);
	Color operator-(Color);
	Color operator*(float);
};

static Color colors[6] = {
	Color(0x00, 0x00, 0xff, 0xff),
	Color(0x00, 0xff, 0x00, 0xff),
	Color(0xff, 0x00, 0x00, 0xff),
	Color(0xff, 0xff, 0x00, 0xff),
	Color(0xff, 0x00, 0xff, 0xff),
	Color(0xff, 0xff, 0xff, 0xff)};

class Point3D
{
  int ID;
  static int counter;
  float x;
  float y;
  float z;

public:
  Point3D();
  Point3D(float, float, float);
  void setX(float);
  void setY(float);
  void setZ(float);
  void rotateQ(float, float, float);
  void rotateX(float);
  void rotateY(float);
  void rotateZ(float);
  void translate(float, float, float);
  void scale(float);
  float dot(Point3D);
  Point3D scaledVector(float);
  Point3D cross(Point3D);
  float getX() { return x; };
  float getY() { return y; };
  float getZ() { return z; };
  void print();
};

class Gradients
{
public:
  float oozXStep, oozYStep;
  float zXStep, zYStep;
  float uXStep, uYStep;
  float vXStep, vYStep;
  Gradients(Point3D[3], float[3], float[3], float[3]);
};

class Edge
{
public:
  float x, xStep;
  int yStart, yEnd;
  float z;
  float zStep;
  float ooz;
  float oozStep;
  float u, v;
  float uStep, vStep;
  Edge();
  Edge(Gradients, Point3D, Point3D, float, float, float);
  void step();
};

class Matrix
{
  float values[16];
  bool onebyX, two, three, four;

public:
  Matrix();
  Matrix(int, int, float, float);
  Matrix(float *, int);
  Matrix(Point3D *);
  void printMatrix();
  int getSize();
  float getValue(int);
  void setSize(int);
  void set2D();
  void set3D();
  void set4D();
  void clear();
  Point3D xPoint(Point3D);
  Matrix projectPoint(Point3D, bool);
  Matrix operator*(Point3D);
  Matrix operator*(Matrix);
};

class Triangle3D
{
  int points[3], texture;

public:
  float uTexels[3], vTexels[3];
  Triangle3D();
  ~Triangle3D();
  int getPoint(int);
  int getP1();
  int getP2();
  int getP3();
  float getUTexel(int);
  float getVTexel(int);
  int getTexture();
  void setP1(int);
  void setP2(int);
  void setP3(int);
  void setUTexel(int, float);
  void setVTexel(int, float);
  void setTexture(int);
};

class Object
{
public:
  char *name;
  Point3D *vertices;
  Triangle3D *triangles;
  int numVertices, numTriangles;
  Object();
  Object(char *);
  ~Object();
  void mallocVertices(Point3D *);
  void mallocTriangles(Triangle3D *);
  void drawObject(float, float, float, float, float, float, Matrix *, Matrix *, float[WIDTH * HEIGHT]);
};

class Quarternion
{
  float w, x, y, z;

public:
  Quarternion();
  Quarternion(Point3D, float);
  Quarternion(float, float, float);
  float getW() { return w; };
  float getX() { return x; };
  float getY() { return y; };
  float getZ() { return z; };
  bool isValid();
  void setW(float);
  void setX(float);
  void setY(float);
  void setZ(float);
  void print();
  Quarternion inverse();
  Quarternion operator*(Quarternion);
  Point3D operator*(Point3D);
};

int dotProduct(int, int, int, int, int, int);
bool triangleOrientation(Point3D[3]);
void scanEdge(Gradients, Edge *, Edge *, bool, float[WIDTH * HEIGHT], int);
void drawTriangle(int, Matrix *, Matrix *, Point3D[3], float[3], float[3], int, float[WIDTH * HEIGHT]);
Matrix viewMatrix(Point3D, Point3D, Point3D, float, float, float);

#endif