#ifndef DATA_IO_INCLUDED
#define DATA_IO_INCLUDED

#include "worldmath.h"
#include "structures.h"

extern Object *objects;
extern int numObjects;
extern struct texture *textures;
extern int numTextures;

void LoadObject(char *);
void LoadTexture(char *);
void LoadPCXFile(char *, struct texture *);
void UnloadData();

#endif