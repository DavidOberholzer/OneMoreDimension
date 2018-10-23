#ifndef WORLDMATH_H_INCLUDED
#define WORLDMATH_H_INCLUDED
#include "SDL2/SDL.h"
#include "graphics.h"

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
  int points[3];

public:
  Triangle3D();
  ~Triangle3D();
  int getPoint(int);
  int getP1();
  int getP2();
  int getP3();
  void setP1(int);
  void setP2(int);
  void setP3(int);
  // bool pointInTriangle(int, int);
};

class Object
{
  char *name;
  Point3D *vertices;
  Triangle3D *triangles;
  int numVertices, numTriangles;

public:
  Object();
  Object(char *);
  ~Object();
  Point3D *getVertices();
  Triangle3D *getTriangles();
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
void drawTriangle(int, Matrix *, Matrix *, Point3D[], float[WIDTH * HEIGHT]);
Matrix viewMatrix(Point3D, Point3D, Point3D, float, float, float);
void drawCube(float, float, float, float, float, float, Matrix *, Matrix *, float[WIDTH * HEIGHT]);
// bool pointInside(int, int, Point3D *, Vector3D *, Vector3D *);

#endif