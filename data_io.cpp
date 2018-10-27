#include <iostream>
#include <stdio.h>

#include "data_io.h"
#include "worldmath.h"
#include "structures.h"

using namespace std;

struct texture *textures = NULL;
int numTextures;

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
	char Buffer[256], word[32], tw[64], *ptr;
	float x, y, z, u1, v1, u2, v2, u3, v3;
	int n, p1, p2, p3, tex;

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
			case 't':
				switch (word[1])
				{
				// Load Textures
				case 'e':
					sscanf(ptr += n, "%64s%n", tw, &n);
					LoadTexture(tw);
					break;
				// Load Triangles
				case 'r':
					o->numTriangles++;
					triangles = (Triangle3D *)realloc(triangles, ++numTriangles * sizeof(*triangles));
					t = &triangles[numTriangles - 1];
					sscanf(ptr += n, "%d%d%d%f%f%f%f%f%f%d%n", &p1, &p2, &p3, &u1, &v1, &u2, &v2, &u3, &v3, &tex, &n);
					t->setP1(p1);
					t->setP2(p2);
					t->setP3(p3);
					t->setUTexel(0, u1);
					t->setUTexel(1, u2);
					t->setUTexel(2, u3);
					t->setVTexel(0, v1);
					t->setVTexel(1, v2);
					t->setVTexel(2, v3);
					t->setTexture(tex);
					break;
				}
			}
		}
	}
	o->mallocVertices(vertices);
	o->mallocTriangles(triangles);
	cout << "Loaded object " << dir << endl;
}

void LoadPCXFile(char *filename, struct texture *texture)
{
	FILE *filePtr;
	unsigned char byte;
	struct pcx_header headerInfo;

	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
	{
		printf("Cannot open file %s\n", filename);
		exit(1);
	}

	fread(&headerInfo, sizeof(struct pcx_header), 1, filePtr);
	if (headerInfo.manufacturer != 10)
	{
		printf("%s File not PCX\n", filename);
		exit(1);
	}
	int imageWidth = headerInfo.Xmax - headerInfo.Xmin + 1;
	int imageHeight = headerInfo.YMax - headerInfo.Ymin + 1;
	texture->width = imageWidth;
	texture->height = imageHeight;
	texture->pixels = (struct pixel *)malloc(sizeof(struct pixel *) * imageWidth * imageHeight);
	int scanLineLength = headerInfo.noBitPlanes * headerInfo.bytesPerRow;
	int colourPerLine = scanLineLength / 3;

	for (int y = 0; y < imageHeight; y++)
	{
		int currentWidth = 0, currentColour = 0, index = 0, count = 0;
		do
		{
			fread(&byte, sizeof(BYTE), 1, filePtr);
			unsigned char runValue;
			int runCount;
			if ((byte & 0xc0) == 0xc0)
			{
				runCount = byte & 0x3f;
				fread(&runValue, sizeof(BYTE), 1, filePtr);
			}
			else
			{
				runCount = 1;
				runValue = byte;
			}
			for (; runCount && index < scanLineLength; runCount--, index++)
			{
				currentColour = (index) / imageWidth;
				currentWidth = index - currentColour * imageWidth;
				int pIndex = y * imageWidth + currentWidth;
				switch (currentColour)
				{
				case 0:
					texture->pixels[pIndex].R = runValue;
					break;
				case 1:
					texture->pixels[pIndex].G = runValue;
					break;
				case 2:
					texture->pixels[pIndex].B = runValue;
					break;
				}
			}
		} while (index < scanLineLength);
	}
	fclose(filePtr);
}

void LoadTexture(char *filename)
{
	char dir[256] = "textures/";
	strcat(dir, filename);
	textures = (struct texture *)realloc(textures, ++numTextures * sizeof(*textures));
	struct texture *texture = &textures[numTextures - 1];
	LoadPCXFile(dir, texture);
	printf("Successfully Loaded texture %s...\n", filename);
}

void UnloadData()
{
	delete objects;
	cout << "Unloaded all data" << endl;
}