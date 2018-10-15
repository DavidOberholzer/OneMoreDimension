#ifndef DATA_IO_INCLUDED
#define DATA_IO_INCLUDED

#include "worldmath.h"

extern Point3D *vertices;
extern Triangle3D *triangles;
extern int numVertices;
extern int numTriangles;

void LoadObject(char *);
void UnloadData();

#endif