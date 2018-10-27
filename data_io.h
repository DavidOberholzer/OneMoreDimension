#ifndef DATA_IO_INCLUDED
#define DATA_IO_INCLUDED

#include "worldmath.h"

extern Object *objects;
extern int numObjects;

void LoadObject(char *);
void LoadTexture(char *);
void LoadPCXFile(char *, struct texture *);
void UnloadData();

#endif