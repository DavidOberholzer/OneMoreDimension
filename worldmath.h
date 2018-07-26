#ifndef WORLDMATH_H_INCLUDED
#define WORLDMATH_H_INCLUDED
#include "SDL2/SDL.h"
#include "graphics.h"

class Point3D
{
  int ID;
  static int counter;
  int x;
  int y;
  int z;

public:
  Point3D();
  Point3D(int, int, int);
  void setX(int);
  void setY(int);
  void setZ(int);
  int getX() { return x; };
  int getY() { return y; };
  int getZ() { return z; };
};

class Vector3D
{
  int ID;
  static int counter;
  Point3D *p1;
  Point3D *p2;
  int vx;
  int vy;
  int vz;

public:
  Vector3D();
  Vector3D(int, int, int, int, int, int);
  ~Vector3D();
  void setP1(int, int, int);
  void setP2(int, int, int);
  Point3D *getP1()
  {
    return p1;
  };
  Point3D *getP2()
  {
    return p2;
  };
  int getVX() { return vx; };
  int getVY() { return vy; };
  int getVZ() { return vz; };
  void draw(Color);
  int operator*(Vector3D *);
};

class Triangle3D
{
  Vector3D *lines = new Vector3D[3];

public:
  Triangle3D(Point3D *, Point3D *, Point3D *);
  ~Triangle3D();
  Vector3D *getVector(int);
  bool pointInTriangle(int, int);
  void draw(Color);
};

class Shape3D
{
  Vector3D *lines;
  int sides;

public:
  Shape3D(int, int *);
  ~Shape3D();
  Vector3D *getVector(int);
  void draw(Color);
};

int dotProduct(int, int, int, int, int, int);
bool pointInside(int, int, Point3D *, Vector3D *, Vector3D *);

#endif