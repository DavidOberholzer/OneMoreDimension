#include <iostream>
#include <stdio.h>

#include "data_io.h"
#include "worldmath.h"

using namespace std;
Object *objects;
int numObjects;
Point3D *vertices;
int numVertices = 0;
Triangle3D *triangles;
int numTriangles = 0;

void LoadObject(char *filename)
{
	Object *o;
	objects = (Object *)realloc(objects, ++numObjects * sizeof(*objects));
	o = &objects[numObjects - 1];
	o->name = filename;
	o->numVertices = 0;
	o->numTriangles = 0;
	char dir[256] = "objects/";
	strcat(dir, filename);
	FILE *data = fopen(dir, "rt");
	if (!data)
	{
		perror("Object did not load!\n");
		exit(1);
	}
	char Buffer[256], word[32], *ptr;
	float x, y, z, u1, v1, u2, v2, u3, v3;
	int n, p1, p2, p3;

	vertices = (Point3D *)malloc(0);
	numVertices = 0;
	triangles = (Triangle3D *)malloc(0);
	numTriangles = 0;
	Point3D *p;
	Triangle3D *t;
	while (fgets(Buffer, sizeof Buffer, data))
	{
		ptr = Buffer;
		if (sscanf(ptr, "%32s%n", word, &n) == 1)
		{
			switch (word[0])
			{
			// Load Vertices
			case 'v':
				o->numVertices++;
				vertices = (Point3D *)realloc(vertices, ++numVertices * sizeof(*vertices));
				p = &vertices[numVertices - 1];
				sscanf(ptr += n, "%f%f%f%n", &x, &y, &z, &n);
				p->setX(x);
				p->setY(y);
				p->setZ(z);
				break;
			// Load Triangles
			case 't':
				o->numTriangles++;
				triangles = (Triangle3D *)realloc(triangles, ++numTriangles * sizeof(*triangles));
				t = &triangles[numTriangles - 1];
				sscanf(ptr += n, "%d%d%d%n", &p1, &p2, &p3, &n);
				t->setP1(p1);
				t->setP2(p2);
				t->setP3(p3);
				break;
			}
		}
	}
	o->mallocVertices(vertices);
	o->mallocTriangles(triangles);
	cout << "Loaded object " << dir << endl;
}

void UnloadData()
{
	delete objects;
	cout << "Unloaded all data" << endl;
}