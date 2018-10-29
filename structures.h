#ifndef STRUCTURES_INCLUDED
#define STRUCTURES_INCLUDED

typedef unsigned char BYTE;
typedef short DBYTE;

// Gradients Structure
struct gradients
{
	float oneOverZ[3];
	float uOverZ[3];
	float vOverZ[3];
	float dOneOverZdX, dOneOverZdY;
	float dUOverZdX, dUOverZdY;
	float dVOverZdX, dVOverZdY;
};

// Pixel Structure
struct pixel
{
	unsigned char R, G, B;
};

// Texture Structure
struct texture
{
	struct pixel *pixels;
	int width, height;
};

#pragma pack(push, 1)

// PCX Header Structure
struct pcx_header
{
	BYTE manufacturer;
	BYTE version;
	BYTE runLength;
	BYTE bpppbp;
	DBYTE Xmin;
	DBYTE Ymin;
	DBYTE Xmax;
	DBYTE YMax;
	DBYTE HorizontalRes;
	DBYTE VerticalRes;
	BYTE pallete[48];
	BYTE reservedB1;
	BYTE noBitPlanes;
	DBYTE bytesPerRow;
	DBYTE palleteInterpretation;
	DBYTE horizontalScreenSize;
	DBYTE verticalScreenSize;
	BYTE reserved[54];
};

#pragma pack(pop)

#endif