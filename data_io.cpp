#include <iostream>
#include <stdio.h>

#include "data_io.h"
#include "worldmath.h"

using namespace std;

Point3D *vertices;
Triangle3D *triangles;
int numVertices, numTriangles;

void LoadObject(char *filename)
{
	char dir[256] = "objects/";
	strcat(dir, filename);
	FILE *data = fopen(dir, "rt");
	if (!data)
	{
		perror("Object did not load!\n");
		exit(1);
	}
	char Buffer[256], word[32], *ptr;
	float x, y, z;
	int n, p1, p2, p3;
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
				vertices = (Point3D *)realloc(vertices, ++numVertices * sizeof(*vertices));
				p = &vertices[numVertices - 1];
				sscanf(ptr += n, "%f%f%f%n", &x, &y, &z, &n);
				p->setX(x);
				p->setY(y);
				p->setZ(z);
				break;
			// Load Triangles
			case 't':
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
	cout << "Loaded object " << dir << endl;
}

void UnloadData()
{
	delete vertices;
	delete triangles;
	cout << "Unloaded all data" << endl;
}