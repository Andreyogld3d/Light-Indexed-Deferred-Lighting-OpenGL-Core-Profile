#ifndef __MESHHEADER_H__
#define __MESHHEADER_H__

#include "types.h"

struct MeshHeader {
	static const uint MeshWithTangent = 1 << 0;
	uchar version;
	uchar numWeights;
	uchar numBones;
	uchar numAnimations;
	ushort numPolygons;
	ushort numVertices;
	float boundingBox[6];
	ubyte flags;
	ubyte reserved1;
	ushort reserved2;
	uint reserved3;
	float StartAnimation;
	float EndAnimation;
	float DeltaTime;
};

#endif //
